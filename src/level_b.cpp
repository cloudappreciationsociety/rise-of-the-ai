#include "level_b.h"

#include "constants.h"
#include "slime.h"

LevelB::LevelB() : Scene {{0.0f, 0.0f}, "#000000"} {}

LevelB::LevelB(Vector2 origin, const char* bgHexCode) : Scene {origin, bgHexCode} {}

LevelB::~LevelB() {
    shutdown();
}

void LevelB::initialize() {
    mGameState.nextSceneId = SceneId::NO_SCENE;

    // mGameState.bgm = LoadMusicStream("assets/game/04 - Silent Forest.wav");
    // SetMusicVolume(mGameState.bgm, 0.33f);
    // PlayMusicStream(mGameState.bgm);

    // mGameState.jumpSound = LoadSound("assets/game/Dirt Jump.wav");

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

    /*
        ----------- PROTAGONIST -----------
    */
    mGameState.player = Entity::create(new Player());
    mGameState.player->mPosition = {constants::SCREEN_CENTER.x, constants::SCREEN_CENTER.y};

    //mGameState.xochitl->setJumpingPower(550.0f);
    mGameState.player->mAcceleration = {0.0f, ACCELERATION_OF_GRAVITY};

    Slime* slime = Entity::create(new Slime(Slime::Type::WANDERER));
    slime->mPosition = {constants::SCREEN_CENTER.x, constants::SCREEN_CENTER.y - 100.0f};
    slime->mAcceleration = {0.0f, ACCELERATION_OF_GRAVITY};
    slime->mTexture.loadAsset(TextureAsset::get("green_slime"));
    slime->spawn(2.0f);

    mGameState.enemies.push_back(slime);
}

void LevelB::update(float deltaTime) {
    // UpdateMusicStream(mGameState.bgm);

    // Have all entities collide with the map
    for (auto& pair : Entity::registry()) {
        if (PhysicsEntity* e = dynamic_cast<PhysicsEntity*>(pair.second.get())) {
            e->mMap = mGameState.map;
        }
    }

    mGameState.player->setCollisionCandidates(this->getPlayerCollisionCandidates());

    Entity::updateAll(deltaTime);

    // if (mGameState.xochitl->getPosition().y > END_GAME_THRESHOLD) {
    //     mGameState.nextSceneId = LEVEL_B;
    // }
}

void LevelB::render() {
    ClearBackground(ColorFromHex(mBgColorHexCode));

    Entity::renderAll();
    mGameState.map->render();
}

void LevelB::shutdown() {
    mGameState.player->markForDeletion();

    delete mGameState.map;
    mGameState.map = nullptr;

    // UnloadMusicStream(mGameState.bgm);
    // mGameState.bgm = {};
    // UnloadSound(mGameState.jumpSound);
    // mGameState.jumpSound = {};
}