#include "level_b.h"

#include "audio.h"
#include "constants.h"
#include "map_objects.h"
#include "slime.h"

LevelB::LevelB() : Scene {{0.0f, 0.0f}, "#000000"} {}

LevelB::LevelB(Vector2 origin, const char* bgHexCode) : Scene {origin, bgHexCode} {
    mHealthBar.position = {125.0f, 50.0f};
    mHealthBar.backgroundColor = BLACK;
    mHealthBar.barColor = RED;
    mHealthBar.textColor = WHITE;
    mHealthBar.size = {200.0f, 40.0f};
    mHealthBar.title = "HP";

    mCoinIcon.mTexture.loadAsset(TextureAsset::get("coin"));
    mCoinIcon.mPosition = {40.0f, 100.0f};
    mCoinIcon.mScale = {40.0f, 40.0f};

    mSlimeSpawns.push_back({Slime::Type::WANDERER, 6, 6});
    mSlimeSpawns.push_back({Slime::Type::WANDERER, 12, 4});
    mSlimeSpawns.push_back({Slime::Type::WANDERER, 18, 6});
    mSlimeSpawns.push_back({Slime::Type::CHASER, 26, 8});
    mSlimeSpawns.push_back({Slime::Type::CHASER, 29, 2});
    mSlimeSpawns.push_back({Slime::Type::CHASER, 12, 2});
    mSlimeSpawns.push_back({Slime::Type::THROWER, 12, 14});

    for (int32_t i = 13; i <= 25; ++i) {
        mSlimeSpawns.push_back({Slime::Type::THROWER, i, 20});
        mSlimeSpawns.push_back({Slime::Type::THROWER, i, 20});
        mSlimeSpawns.push_back({Slime::Type::THROWER, i, 20});
        mSlimeSpawns.push_back({Slime::Type::WANDERER, i, 20});
    }
}

LevelB::~LevelB() {
    shutdown();
}

void LevelB::initialize() {
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

    mGameState.bg = Entity::create(new Entity(TextureAsset::get("bg_b")));
    mGameState.bg->mPosition = constants::SCREEN_CENTER;
    mGameState.bg->mScale = Vector2Scale(constants::SCREEN_SIZE, 2.3f);

    /*
        ----------- PROTAGONIST -----------
    */
    mGameState.player = Entity::create(new Player());
    mGameState.player->mPosition =
        mGameState.map->getCenterOfTile(LevelB::PLAYER_SPAWN_COL, LevelB::PLAYER_SPAWN_ROW);
    mGameState.player->mAcceleration = {0.0f, ACCELERATION_OF_GRAVITY};

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

    // Coins
    for (auto coinPosition : mCoins) {
        Coin* coin = Entity::create(new Coin);
        coin->mPosition = mGameState.map->getCenterOfTile(
            static_cast<int32_t>(coinPosition.x),
            static_cast<int32_t>(coinPosition.y)
        );

        mGameState.objects.push_back(coin);
    }
    mCoinsRemaining = LevelB::COIN_COUNT;

    Audio::get().playMusic("bg");
}

void LevelB::update(float deltaTime) {
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
        mGameState.nextSceneId = SceneId::LEVEL_C;
    }
}

void LevelB::render() {
    ClearBackground(ColorFromHex(mBgColorHexCode));

    Entity::renderAll();
    mGameState.map->render();
}

void LevelB::renderUI() {
    mHealthBar.render(mGameState.player->mHealth, mGameState.player->mMaxHealth);
    mCoinIcon.render();

    drawText(TextFormat("%i remaining", mCoinsRemaining), {130.0f, 100.0f}, 24, WHITE);
}

void LevelB::shutdown() {
    Scene::shutdown();
}