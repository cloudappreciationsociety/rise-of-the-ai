#ifndef LEVEL_B_H
#define LEVEL_B_H

#include "scene.h"

class LevelB: public Scene {
private:
    static constexpr uint32_t LEVEL_WIDTH = 14;
    static constexpr uint32_t LEVEL_HEIGHT = 8;

private:
    // clang-format off
    int32_t mLevelData[LEVEL_WIDTH * LEVEL_HEIGHT] = {
        1, 1, 1, 1, 1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1
    };
    // clang-format on

public:
    static constexpr float TILE_DIMENSION = 75.0f;
    static constexpr float ACCELERATION_OF_GRAVITY = 981.0f;
    static constexpr float END_GAME_THRESHOLD = 800.0f;

public:
    LevelB();
    LevelB(Vector2 origin, const char* bgHexCode);
    ~LevelB();

    void initialize() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif