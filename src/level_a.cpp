#include "level_a.h"

#include "audio.h"
#include "constants.h"
#include "map_objects.h"
#include "slime.h"

LevelA::LevelA() : Scene {{0.0f, 0.0f}, "#000000"} {}

LevelA::LevelA(Vector2 origin, const char* bgHexCode) : Scene {origin, bgHexCode} {
    mHealthBar.position = {125.0f, 50.0f};
    mHealthBar.backgroundColor = BLACK;
    mHealthBar.barColor = RED;
    mHealthBar.textColor = WHITE;
    mHealthBar.size = {200.0f, 40.0f};
    mHealthBar.title = "HP";

    mCoinIcon.mTexture.loadAsset(TextureAsset::get("coin"));
    mCoinIcon.mPosition = {40.0f, 100.0f};
    mCoinIcon.mScale = {40.0f, 40.0f};
}

LevelA::~LevelA() {
    shutdown();
}

void LevelA::initialize() {
    Scene::initialize();

    /*
        ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVEL_WIDTH,
        LEVEL_HEIGHT, // map grid cols & rows
        mLevelData, // grid data
        TILE_DIMENSION, // tile size
        "map_tiles", // texture filepath
        mOrigin // in-game origin
    );

    mGameState.bg = Entity::create(new Entity(TextureAsset::get("bg_a")));
    mGameState.bg->mPosition = constants::SCREEN_CENTER;
    mGameState.bg->mScale = Vector2Scale(constants::SCREEN_SIZE, 2.0f);

    /*
        ----------- PROTAGONIST -----------
    */
    mGameState.player = Entity::create(new Player());
    mGameState.player->mPosition =
        mGameState.map->getCenterOfTile(LevelA::PLAYER_SPAWN_COL, LevelA::PLAYER_SPAWN_ROW);
    mGameState.player->mAcceleration = {0.0f, ACCELERATION_OF_GRAVITY};

    // Coins
    for (auto coinPosition : mCoins) {
        Coin* coin = Entity::create(new Coin);
        coin->mPosition = mGameState.map->getCenterOfTile(
            static_cast<int32_t>(coinPosition.x),
            static_cast<int32_t>(coinPosition.y)
        );

        mGameState.objects.push_back(coin);
    }
    mCoinsRemaining = LevelA::COIN_COUNT;

    // Slimes
    for (const SlimeSpawn& spawn : mSlimeSpawns) {
        Slime* slime = Entity::create(new Slime(spawn.type));
        slime->mTexture.loadAsset(
            TextureAsset::get(spawn.type == Slime::Type::CHASER ? "purple_slime" : "green_slime")
        );
        slime->mPosition = mGameState.map->getCenterOfTile(spawn.col, spawn.row);
        slime->mTarget = mGameState.player;
        slime->mAcceleration = {0.0f, ACCELERATION_OF_GRAVITY};
        slime->spawn(1.0f);

        mGameState.enemies.push_back(slime);
    }

    Audio::get().playMusic("bg");
}

void LevelA::update(float deltaTime) {
    Audio::get().update();

    // Have all entities collide with the map
    for (auto& pair : Entity::registry()) {
        if (PhysicsEntity* e = dynamic_cast<PhysicsEntity*>(pair.second.get())) {
            e->mMap = mGameState.map;
        }
    }

    mGameState.player->setCollisionCandidates(this->getPlayerCollisionCandidates());

    Entity::updateAll(deltaTime);

    for (auto entity : mGameState.objects) {
        if (!mGameState.player->isColliding(entity)) {
            continue;
        }

        if (Coin* coin = dynamic_cast<Coin*>(entity)) {
            coin->deactivate();
            --mCoinsRemaining;

            Audio::get().playSound("collect");
        }
    }

    if (!mGameState.player->isAlive()) {
        Audio::get().stopActiveMusic();
        mGameState.nextSceneId = SceneId::LOSE;
    }

    if (mCoinsRemaining == 0) {
        Audio::get().playSound("level_complete");
        mGameState.nextSceneId = SceneId::LEVEL_B;
    }
}

void LevelA::render() {
    ClearBackground(ColorFromHex(mBgColorHexCode));

    Entity::renderAll();
    mGameState.map->render();
}

void LevelA::renderUI() {
    mHealthBar.render(mGameState.player->mHealth, mGameState.player->mMaxHealth);
    mCoinIcon.render();

    drawText(TextFormat("%i remaining", mCoinsRemaining), {130.0f, 100.0f}, 24, WHITE);
}

void LevelA::shutdown() {
    Scene::shutdown();
}