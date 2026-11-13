#ifndef LEVEL_B_H
#define LEVEL_B_H

#include "scene.h"
#include "slime.h"
#include "ui.h"

class LevelB: public Scene {
private:
    static constexpr uint32_t LEVEL_WIDTH = 36;
    static constexpr uint32_t LEVEL_HEIGHT = 14;

    enum : int32_t {
        _ = -1, // Empty
        T = 2, // Sand with a lit top edge
        B = 3, // Sand brick (platforms)
        F = 18, // Plain sand
        W = 19 // Cobbled sand
    };

private:
    // clang-format off
    int32_t mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W,
        W, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, W,
        W, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, W,
        W, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, W,
        W, _, _, _, _, _, _, _, _, _, _, _, _, _, B, B, B, B, B, B, B, B, _, _, _, _, _, _, _, _, _, _, _, _, _, W,
        W, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, W,
        W, _, _, _, _, _, _, _, _, B, B, B, B, _, _, _, _, _, _, _, _, _, _, B, B, B, B, _, _, _, _, _, _, _, _, W,
        W, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, W,
        W, _, _, _, B, B, B, B, _, _, _, _, _, _, _, B, B, B, B, B, B, _, _, _, _, _, _, _, B, B, B, B, _, _, _, W,
        W, _, _, _, _, _, _, _, B, B, _, _, _, B, B, B, B, B, B, B, B, B, B, _, _, _, B, B, _, _, _, _, _, _, _, W,
        W, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, W,
        W, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, W,
        W, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, W,
        W, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, W,
    };
    // clang-format on

public:
    static constexpr float TILE_DIMENSION = 60.0f;
    static constexpr float ACCELERATION_OF_GRAVITY = 981.0f;
    static constexpr float END_GAME_THRESHOLD = 800.0f;

private:
    static constexpr int32_t PLAYER_SPAWN_COL = 2;
    static constexpr int32_t PLAYER_SPAWN_ROW = 9;

    static constexpr uint32_t SLIME_COUNT = 6;
    const SlimeSpawn mSlimeSpawns[SLIME_COUNT] = {
        {Slime::Type::WANDERER, 6, 7},
        {Slime::Type::WANDERER, 12, 5},
        {Slime::Type::WANDERER, 18, 7},
        {Slime::Type::CHASER, 26, 9},
        {Slime::Type::CHASER, 29, 3},
        {Slime::Type::THROWER, 12, 3}
    };

    static constexpr uint32_t COIN_COUNT = 3;
    const Vector2 mCoins[COIN_COUNT] = {{28, 4}, {7, 8}, {15, 4}};
    uint32_t mCoinsRemaining = LevelB::COIN_COUNT;

    ProgressBar mHealthBar;

public:
    LevelB();
    LevelB(Vector2 origin, const char* bgHexCode);
    ~LevelB();

    void initialize() override;
    void update(float deltaTime) override;
    void render() override;
    void renderUI() override;
    void shutdown() override;
};

#endif