#ifndef LEVEL_C_H
#define LEVEL_C_H

#include "scene.h"
#include "slime.h"
#include "ui.h"

class LevelC: public Scene {
private:
    static constexpr uint32_t LEVEL_WIDTH = 32;
    static constexpr uint32_t LEVEL_HEIGHT = 30;

    enum : int32_t {
        _ = -1, // Empty
        T = 6, // Stone topped with snow
        S = 22, // Stone
        C = 8 // Cobbled stone
    };

private:
    // clang-format off
    int32_t mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, T, T, T, T, T, T, T, T, _, T, T, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, T, _, S, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, S, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, T, _, _, _, T, _, _, S, S, _, _, C, C, C, C, C, C, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, T, _, _, _, _, S, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, T, T, T, _, _, _, _, _, _, _, _, S, S, _, S, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, S, _, _, _, _, _, C, C, C, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, T, T, T, T, _, _, _, _, S, _, _, _, _, _, _, _, C, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, S, T, _, _, _, S, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, S, S, S, S, _, _, S, _, _, S, S, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, S, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, T, S, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, T, _, _, _, _, _, _, _, _, _, _, _, T, S, S, C, _, C, C, C, C, C, C, _, C, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, S, S, _, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, S, S, _, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, T, _, _, _, _, _, _, T, T, S, S, _, S, S, S, S, S, _, S, S, S, S, C,
        C, T, _, _, _, _, _, _, _, _, T, _, _, T, _, _, S, S, S, S, _, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, T, _, _, _, _, _, _, _, S, S, S, _, _, _, _, S, _, _, _, _, _, C, _, C,
        C, _, _, _, _, _, _, _, _, _, T, T, T, T, T, _, S, S, _, _, S, _, _, _, _, C, C, _, _, _, _, C,
        C, _, _, _, _, _, T, T, _, _, _, _, _, _, _, T, S, S, _, _, _, _, S, _, _, _, _, _, S, S, S, C,
        C, _, _, _, T, T, S, S, S, S, S, S, S, S, S, S, S, _, S, S, _, _, _, _, _, _, S, _, _, _, S, C,
        C, _, _, T, S, S, S, S, S, S, S, S, S, S, S, S, S, _, _, _, _, _, _, C, _, _, _, _, _, _, S, C,
        C, T, T, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, S, C,
        C, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, C,
    };
    // clang-format on

public:
    static constexpr float TILE_DIMENSION = 60.0f;
    static constexpr float ACCELERATION_OF_GRAVITY = 981.0f;
    static constexpr float END_GAME_THRESHOLD = 800.0f;

private:
    static constexpr int32_t PLAYER_SPAWN_COL = 1;
    static constexpr int32_t PLAYER_SPAWN_ROW = 27;

    std::vector<SlimeSpawn> mSlimeSpawns = {
        {Slime::Type::THROWER, 1, 21},
        {Slime::Type::CHASER, 10, 25},
        {Slime::Type::THROWER, 6, 17},
        {Slime::Type::WANDERER, 17, 20},
        {Slime::Type::WANDERER, 19, 16},
        {Slime::Type::WANDERER, 14, 13},
        {Slime::Type::WANDERER, 13, 13},
        {Slime::Type::THROWER, 9, 10},
        {Slime::Type::CHASER, 30, 5},
        {Slime::Type::THROWER, 21, 7},
        {Slime::Type::WANDERER, 24, 8},
        {Slime::Type::WANDERER, 25, 8},
        {Slime::Type::WANDERER, 26, 8},
        {Slime::Type::CHASER, 23, 10},
        {Slime::Type::THROWER, 21, 10},
        {Slime::Type::WANDERER, 26, 11},
    };

    static constexpr uint32_t COIN_COUNT = 1;
    const Vector2 mCoins[COIN_COUNT] = {{3, 28}};
    uint32_t mCoinsRemaining = LevelC::COIN_COUNT;

    ProgressBar mHealthBar;
    Entity mCoinIcon;

    bool mFinalWaveSpawned = false;

public:
    LevelC();
    LevelC(Vector2 origin, const char* bgHexCode);
    ~LevelC();

    void initialize() override;
    void update(float deltaTime) override;
    void render() override;
    void renderUI() override;
    void shutdown() override;
};

#endif