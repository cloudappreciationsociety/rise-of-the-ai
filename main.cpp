/**
* Author: Alan Chen
* Assignment: Lunar Lander
* Date due: 10/26/2026
*
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include <map>
#include <memory>
#include <random>

#include "constants.h"
#include "entity.h"
#include "lib.h"
#include "ui.h"

enum class GameStatus { IN_MENU, STARTING, IN_PROGRESS, ENDED };

AppStatus gAppStatus = AppStatus::RUNNING;
GameStatus gGameStatus = GameStatus::IN_MENU;

float gTimeAccumulator = 0.0f;
float gPreviousTimestampSec = 0.0f;

Color gBackgroundColor = SKYBLUE;

void resetGame() {
    // Delete
    for (auto& pair : Entity::registry()) {
        auto ptr = pair.second.get();

        // ptr->markForDeletion();
    }
}

void initialize() {
    InitWindow(constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, "Save the Chinese Spy Balloon");

    SetTargetFPS(constants::TARGET_FPS);

    // Map

    // Textures

    // Entities

    // Vector2 tileSize = {50.0f, 50.0f};
    // uint32_t tileCount = 2 * constants::SCREEN_WIDTH / tileSize.x;

    // float leftMostX = constants::SCREEN_CENTER.x - (tileCount * tileSize.x) / 2.0f;

    // for (uint32_t i = 0; i < tileCount; ++i) {
    //     auto tile = Entity::create(new PhysicsEntity(gGrassTileTexture));
    //     tile->scale = tileSize;
    //     tile->colliderDimensions = tile->scale;
    //     tile->position = {leftMostX + i * tileSize.x, constants::SCREEN_HEIGHT - tileSize.y / 2.0f};
    //     tile->anchored = true;

    //     gMapTiles.push_back(tile);
    // }

    // Backgrounds
    // Sets player positions, zeroes out scores, etc.
    resetGame();

    // Audio

    // UI elements

    gPreviousTimestampSec = static_cast<float>(GetTime());
}

void processInput() {
    Vector2 mousePosition = GetMousePosition();

    switch (gGameStatus) {
        case (GameStatus::IN_MENU): {
            if (gPlayButton.leftClicked()) {
                resetGame();
                gGameStatus = GameStatus::STARTING;
            }

            if (gQuitButton.leftClicked()) {
                gAppStatus = AppStatus::TERMINATED;
            }

            break;
        }
        case (GameStatus::STARTING): {
            // No inputs possible
            break;
        }
        case (GameStatus::IN_PROGRESS): {
            break;
        }
        case (GameStatus::ENDED): {
            if (gBackToMenuButton.leftClicked()) {
                resetGame();
                gGameStatus = GameStatus::IN_MENU;
            }

            if (gQuitButton.leftClicked()) {
                gAppStatus = AppStatus::TERMINATED;
            }

            break;
        }
        default:
            break;
    }

    if (IsKeyDown(KEY_Q) || WindowShouldClose()) {
        gAppStatus = AppStatus::TERMINATED;
    }
}

void updateGameMenu() {}

void updateGameStarting(float deltaTime) {}

void updateGameInProgress(float deltaTime) {
    // Set collision candidates
    for (auto& pair : Entity::registry()) {
        Entity* ptr = pair.second.get();
    }

    for (auto& pair : Entity::registry()) {
        pair.second->update(deltaTime);
    }
}

void update() {
    float nowSec = static_cast<float>(GetTime());
    float deltaTime = nowSec - gPreviousTimestampSec;
    gPreviousTimestampSec = nowSec;

    deltaTime += gTimeAccumulator; // fold in any leftover from last frame

    if (deltaTime < constants::FIXED_TIMESTEP) {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= constants::FIXED_TIMESTEP) {
        // Delete objects marked for deletion
        // Ugly C++11 iterate-and-erase stuff
        for (auto it = Entity::registry().begin(); it != Entity::registry().end();) {
            if (it->second->isMarkedForDeletion()) {
                it = Entity::registry().erase(it);
            } else {
                ++it;
            }
        }

        switch (gGameStatus) {
            case (GameStatus::IN_MENU): {
                updateGameMenu();
                break;
            }
            case (GameStatus::STARTING): {
                updateGameStarting(constants::FIXED_TIMESTEP);
                break;
            }
            case (GameStatus::IN_PROGRESS): {
                updateGameInProgress(constants::FIXED_TIMESTEP);
                break;
            }
            case (GameStatus::ENDED): {
                break;
            }
            default:
                break;
        }

        deltaTime -= constants::FIXED_TIMESTEP;
    }

    gTimeAccumulator = deltaTime;
}

void render() {
    BeginDrawing();

    ClearBackground(gBackgroundColor);

    for (const auto& pair : Entity::registry()) {
        pair.second->render();
    }

    EndDrawing();
}

void shutdown() {
    Entity::registry().clear();
    TextureAsset::registry().clear();

    CloseWindow();
}

int main() {
    initialize();

    while (gAppStatus == AppStatus::RUNNING) {
        processInput();
        update();
        render();
    }

    shutdown();

    return 0;
}