/**
* Author: Alan Chen
* Assignment: Rise of the AI
* Date due: 11/16/2026
*
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include <map>

#include "constants.h"
#include "entity.h"
#include "level_a.h"
#include "lib.h"
#include "player.h"
#include "scene.h"
#include "slime.h"
#include "ui.h"

AppStatus gAppStatus = AppStatus::RUNNING;
Camera2D gCamera = {};

float gTimeAccumulator = 0.0f;
float gPreviousTimestampSec = 0.0f;

Scene* gCurrentScene = nullptr;
std::map<SceneId, Scene*> gLevels = {};

/**
 * @brief Smoothly adjusts the camera's target position towards a specified
 * target position.
 *
 * @param camera A pointer to a `Camera2D` struct.
 * @param targetPosition The position the camera should move towards.
 */
void panCamera(Camera2D* camera, Vector2 targetPosition) {
    Vector2 positionDifference = Vector2Subtract(targetPosition, camera->target);

    camera->target = Vector2Add(
        camera->target,
        Vector2Scale(positionDifference, 0.1f)
    ); // 0.1 = smoothing factor
}

void switchToScene(Scene* scene) {
    if (gCurrentScene) {
        gCurrentScene->shutdown();
    }

    gCurrentScene = scene;
    gCurrentScene->initialize();
    gCamera.target = gCurrentScene->getCameraTarget();
}

void initialize() {
    InitWindow(constants::SCREEN_WIDTH, constants::SCREEN_HEIGHT, "Knight vs. Slimes");
    InitAudioDevice();

    SetTargetFPS(constants::TARGET_FPS);

    // Textures
    auto map =
        TextureAsset::create(new TextureAsset("map_tiles", "assets/textures/world_tileset.png"));
    map->mType = TextureType::ATLAS;
    map->mSpriteSheetDimensions = {16, 16};

    auto knight = TextureAsset::create(new TextureAsset("knight", "assets/textures/knight.png"));
    knight->mType = TextureType::ATLAS;
    knight->mSpriteSheetDimensions = {8, 8};
    knight->mAnimationSpriteSheetFrames = {
        {static_cast<int32_t>(Player::AnimationState::IDLE), {0, 1, 2, 3}},
        {static_cast<int32_t>(Player::AnimationState::MOVING),
         {16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31}},
        {static_cast<int32_t>(Player::AnimationState::JUMPING), {40, 41, 42, 43, 44, 45, 46, 47}},
        {static_cast<int32_t>(Player::AnimationState::DEAD), {56, 57, 58, 59}},
    };

    auto greenSlime =
        TextureAsset::create(new TextureAsset("green_slime", "assets/textures/slime_green.png"));
    greenSlime->mType = TextureType::ATLAS;
    greenSlime->mSpriteSheetDimensions = {3, 4};
    greenSlime->mAnimationSpriteSheetFrames = {
        {static_cast<int32_t>(Slime::AnimationState::SPAWNING), {0, 1, 2, 3}},
        {static_cast<int32_t>(Slime::AnimationState::ACTIVE), {4, 5, 6, 7}}
    };

    auto purpleSlime =
        TextureAsset::create(new TextureAsset("purple_slime", "assets/textures/slime_purple.png"));
    purpleSlime->mType = TextureType::ATLAS;
    purpleSlime->mSpriteSheetDimensions = {3, 4};
    purpleSlime->mAnimationSpriteSheetFrames = {
        {static_cast<int32_t>(Slime::AnimationState::SPAWNING), {0, 1, 2, 3}},
        {static_cast<int32_t>(Slime::AnimationState::ACTIVE), {4, 5, 6, 7}}
    };

    // Entities

    // Levels
    gLevels[SceneId::LEVEL_A] = new LevelA(constants::SCREEN_CENTER, "#000000");
    switchToScene(gLevels[SceneId::LEVEL_A]);

    gCamera.offset = constants::SCREEN_CENTER;
    gCamera.rotation = 0.0f;
    gCamera.zoom = 1.0f;

    gPreviousTimestampSec = static_cast<float>(GetTime());
}

void processInput() {
    Vector2 mousePosition = GetMousePosition();

    if (Player* player = gCurrentScene->getState().player) {
        player->resetMovement();

        if (IsKeyDown(KEY_A)) {
            player->moveLeft();
        } else if (IsKeyDown(KEY_D)) {
            player->moveRight();
        }

        if (IsKeyPressed(KEY_W) && player->mIsCollidingBottom) {
            player->jump();
        }
    }

    if (IsKeyDown(KEY_Q) || WindowShouldClose()) {
        gAppStatus = AppStatus::TERMINATED;
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

        gCurrentScene->update(constants::FIXED_TIMESTEP);
        panCamera(&gCamera, gCurrentScene->getCameraTarget());
        deltaTime -= constants::FIXED_TIMESTEP;
    }

    gTimeAccumulator = deltaTime;
}

void render() {
    BeginDrawing();
    BeginMode2D(gCamera);

    gCurrentScene->render();

    EndMode2D();
    EndDrawing();
}

void shutdown() {
    for (std::pair<const SceneId, Scene*>& entry : gLevels) {
        delete entry.second;
    }

    gLevels.clear();

    Entity::registry().clear();
    TextureAsset::registry().clear();

    CloseAudioDevice();
    CloseWindow();
}

int main() {
    initialize();

    while (gAppStatus == AppStatus::RUNNING) {
        processInput();
        update();

        SceneId nextScene = gCurrentScene->getState().nextSceneId;
        if (nextScene != SceneId::NO_SCENE) {
            switchToScene(gLevels[nextScene]);
        }

        render();
    }

    shutdown();

    return 0;
}