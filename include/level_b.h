#ifndef LEVEL_B_H
#define LEVEL_B_H

#include "scene.h"
#include "slime.h"
#include "ui.h"

class LevelB: public Scene {
private:
    static constexpr uint32_t LEVEL_WIDTH = 34;
    static constexpr uint32_t LEVEL_HEIGHT = 22;

    enum : int32_t {
        _ = -1, // Empty
        T = 2, // Sand with a lit top edge
        B = 3, // Sand brick
        F = 18, // Plain sand
        W = 19 // Cobbled sand
    };

private:
    // clang-format off
    int32_t mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        W, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, W,
        W, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, W,
        W, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, W,
        W, _, _, _, _, _, _, _, _, _, _, _, _, _, B, B, B, B, B, B, B, B, _, _, _, _, _, _, _, _, _, _, _, W,
        W, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, W,
        W, _, _, _, _, _, _, _, _, B, B, B, B, _, _, _, _, _, _, _, _, _, _, B, B, B, B, _, _, _, _, _, _, W,
        W, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, W,
        W, _, _, _, B, B, B, _, _, _, _, _, _, _, _, B, B, B, B, B, B, _, _, _, _, _, _, _, B, B, B, B, _, W,
        W, _, _, _, _, _, _, _, B, B, _, _, _, B, B, B, B, B, B, B, B, B, B, _, _, _, B, B, _, _, _, _, _, W,
        W, T, T, T, T, T, T, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, T, T, T, W,
        W, F, F, F, F, F, F, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, F, F, F, W,
        W, F, F, F, F, F, F, F, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, F, F, F, W,
        W, F, F, F, F, F, _, _, _, _, _, _, _, _, _, _, _, _, _, B, _, _, _, _, _, _, _, _, _, _, _, _, F, W,
        W, F, F, F, F, F, F, F, F, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, F, F, F, W,
        W, F, F, F, F, F, F, F, F, F, F, _, _, _, _, _, B, _, _, _, _, B, _, _, _, _, _, F, F, F, F, F, F, W,
        W, F, F, F, F, F, F, F, F, F, F, F, F, _, _, _, _, _, _, _, _, _, _, _, _, _, _, F, F, F, F, F, F, W,
        W, F, F, F, F, F, F, F, F, F, F, F, F, _, _, _, _, _, _, _, _, _, _, B, _, _, _, F, F, F, F, F, F, W,
        W, F, F, F, F, F, F, F, F, F, F, F, F, _, _, _, _, _, _, _, _, _, _, B, B, _, _, _, _, F, F, F, F, W,
        W, F, F, F, F, F, F, F, F, F, F, F, F, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, F, F, F, F, F, W,
        W, F, F, F, F, F, F, F, F, F, F, F, F, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, F, F, F, F, F, W,
        W, F, F, F, F, F, F, F, F, F, F, F, F, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, F, F, F, F, F, W,
        W, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, F, W
    };
    // clang-format on

public:
    static constexpr float TILE_DIMENSION = 60.0f;
    static constexpr float ACCELERATION_OF_GRAVITY = 981.0f;

private:
    static constexpr int32_t PLAYER_SPAWN_COL = 2;
    static constexpr int32_t PLAYER_SPAWN_ROW = 9;

    std::vector<SlimeSpawn> mSlimeSpawns;

    static constexpr uint32_t COIN_COUNT = 6;
    const Vector2 mCoins[COIN_COUNT] = {{6, 12}, {21, 13}, {28, 3}, {28, 17}, {17, 6}, {31, 12}};
    uint32_t mCoinsRemaining = LevelB::COIN_COUNT;

    ProgressBar mHealthBar;
    Entity mCoinIcon;

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