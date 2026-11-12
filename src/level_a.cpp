#include "level_a.h"

#include "constants.h"
#include "slime.h"

LevelA::LevelA() : Scene {{0.0f, 0.0f}, "#000000"} {}

LevelA::LevelA(Vector2 origin, const char* bgHexCode) : Scene {origin, bgHexCode} {}

LevelA::~LevelA() {
    shutdown();
}

void LevelA::initialize() {
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
    mGameState.player->mPosition =
        mGameState.map->getCenterOfTile(LevelA::PLAYER_SPAWN_COL, LevelA::PLAYER_SPAWN_ROW);
    mGameState.player->mAcceleration = {0.0f, ACCELERATION_OF_GRAVITY};

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
}

void LevelA::update(float deltaTime) {
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

void LevelA::render() {
    ClearBackground(ColorFromHex(mBgColorHexCode));

    Entity::renderAll();
    mGameState.map->render();
}

void LevelA::shutdown() {
    mGameState.player->markForDeletion();

    delete mGameState.map;
    mGameState.map = nullptr;

    // UnloadMusicStream(mGameState.bgm);
    // mGameState.bgm = {};
    // UnloadSound(mGameState.jumpSound);
    // mGameState.jumpSound = {};
}