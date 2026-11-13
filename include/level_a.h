#ifndef LEVEL_A_H
#define LEVEL_A_H

#include "scene.h"
#include "slime.h"
#include "ui.h"

class LevelA: public Scene {
private:
    static constexpr uint32_t LEVEL_WIDTH = 32;
    static constexpr uint32_t LEVEL_HEIGHT = 16;

    enum : int32_t {
        _ = -1, // Empty
        G = 0, // Grassy dirt
        D = 16, // Dirt
        C = 17 // Cobbled dirt
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
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, G, G, G, G, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, _, G, G, G, G, G, _, _, _, _, _, _, _, G, G, G, G, G, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, G, G, G, G, G, _, _, _, _, _, _, _, G, G, G, G, G, _, _, _, _, _, _, _, _, _, _, C,
        C, _, _, _, _, _, _, _, _, _, _, G, G, _, _, _, _, _, _, _, _, _, _, G, G, _, _, _, _, _, _, C,
        C, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, G, C,
        C, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, C,
        C, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, C,
        C, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, D, C
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

    static constexpr uint32_t COIN_COUNT = 4;
    const Vector2 mCoins[COIN_COUNT] = {{28, 4}, {7, 8}, {10, 4}, {15, 4}};
    uint32_t mCoinsRemaining = LevelA::COIN_COUNT;

    ProgressBar mHealthBar;
    TextureInstance mCoinIcon;

public:
    LevelA();
    LevelA(Vector2 origin, const char* bgHexCode);
    ~LevelA();

    void initialize() override;
    void update(float deltaTime) override;
    void render() override;
    void renderUI() override;
    void shutdown() override;
};

#endif