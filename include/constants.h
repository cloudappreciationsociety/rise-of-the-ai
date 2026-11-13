#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <cstdint>

#include "raylib.h"

namespace constants {

constexpr int32_t SCREEN_WIDTH = 1000;
constexpr int32_t SCREEN_HEIGHT = 600;
constexpr int32_t TARGET_FPS = 60;
constexpr Vector2 SCREEN_CENTER = {SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
constexpr Vector2 SCREEN_SIZE = {
    static_cast<float>(SCREEN_WIDTH),
    static_cast<float>(SCREEN_HEIGHT)
};

constexpr float FIXED_TIMESTEP = 1.0f / TARGET_FPS;

constexpr Color BLACK_TINT = {0, 0, 0, 180};

namespace ui {
constexpr int32_t HEADER_FONT_SIZE = 64;
constexpr int32_t FONT_SIZE = 32;
} // namespace ui

} // namespace constants

#endif