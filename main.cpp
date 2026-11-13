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

#include "audio.h"
#include "constants.h"
#include "level_a.h"
#include "level_b.h"
#include "level_c.h"
#include "lib.h"
#include "menus.h"

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

void clampCamera(Camera2D* camera, Rectangle bounds) {
    if (bounds.width <= 0.0f || bounds.height <= 0.0f) {
        return;
    }

    // Current camera extents
    float leftExtent = camera->offset.x / camera->zoom;
    float rightExtent = (GetScreenWidth() - camera->offset.x) / camera->zoom;
    float topExtent = camera->offset.y / camera->zoom;
    float bottomExtent = (GetScreenHeight() - camera->offset.y) / camera->zoom;

    float minX = bounds.x + leftExtent;
    float maxX = bounds.x + bounds.width - rightExtent;
    float minY = bounds.y + topExtent;
    float maxY = bounds.y + bounds.height - bottomExtent;

    if (minX > maxX) {
        camera->target.x = bounds.x + bounds.width / 2.0f - (rightExtent - leftExtent) / 2.0f;
    } else {
        camera->target.x = std::min(std::max(camera->target.x, minX), maxX);
    }

    if (minY > maxY) {
        camera->target.y = bounds.y + bounds.height / 2.0f - (bottomExtent - topExtent) / 2.0f;
    } else {
        camera->target.y = std::min(std::max(camera->target.y, minY), maxY);
    }
}

void switchToScene(Scene* scene) {
    if (gCurrentScene) {
        gCurrentScene->shutdown();
    }

    gCurrentScene = scene;
    gCurrentScene->initialize();
    gCamera.target = gCurrentScene->getCameraTarget();
    if (Map* map = gCurrentScene->getState().map) {
        clampCamera(&gCamera, map->getBounds());
    }
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

    auto coin = TextureAsset::create(new TextureAsset("coin", "assets/textures/coin.png"));
    coin->mType = TextureType::ATLAS;
    coin->mSpriteSheetDimensions = {1, 12};
    coin->useDefaultAnimation();

    TextureAsset::create(new TextureAsset("bg_a", "assets/textures/bg-a.png"));
    TextureAsset::create(new TextureAsset("bg_b", "assets/textures/bg-b.png"));
    TextureAsset::create(new TextureAsset("bg_c", "assets/textures/bg-c.png"));
    TextureAsset::create(new TextureAsset("slime_ball", "assets/textures/slime_ball.png"));

    // Audio
    Audio::get().loadSound("knight_jump", "assets/audio/knight-jump.mp3");
    Audio::get().loadSound("knight_hurt", "assets/audio/knight-ow.mp3");
    Audio::get().loadSound("knight_death", "assets/audio/knight-death.mp3");
    Audio::get().loadSound("slime_jump", "assets/audio/slime-jump.mp3");
    Audio::get().loadSound("slime_death", "assets/audio/slime-death.mp3");
    Audio::get().loadSound("collect", "assets/audio/collect.mp3");
    Audio::get().loadSound("slime_throw", "assets/audio/slime-throw.mp3");
    Audio::get().loadSound("slime_ball_impact", "assets/audio/slime-ball-impact.mp3");
    Audio::get().loadSound("win", "assets/audio/win.mp3");
    Audio::get().loadSound("lose", "assets/audio/lose.mp3");
    Audio::get().loadSound("level_complete", "assets/audio/level-complete.mp3");

    Audio::get().loadMusic("start_menu", "assets/audio/menu-bg-music.mp3", 1.0f);
    Audio::get().loadMusic("bg", "assets/audio/autumn-leaves.mp3", 1.0f);

    // Levels
    gLevels[SceneId::START] = new StartMenu;
    gLevels[SceneId::LEVEL_A] = new LevelA(constants::SCREEN_CENTER, "#000000");
    gLevels[SceneId::LEVEL_B] = new LevelB(constants::SCREEN_CENTER, "#000000");
    gLevels[SceneId::LEVEL_C] = new LevelC(constants::SCREEN_CENTER, "#000000");
    gLevels[SceneId::WIN] = new WinMenu;
    gLevels[SceneId::LOSE] = new LoseMenu;
    switchToScene(gLevels[SceneId::START]);

    gCamera.offset = constants::SCREEN_CENTER;
    gCamera.rotation = 0.0f;
    gCamera.zoom = 1.0f;

    gPreviousTimestampSec = static_cast<float>(GetTime());
}

void processInput() {
    Vector2 mousePosition = GetMousePosition();

    if (Player* player = gCurrentScene->getState().player) {
        player->resetMovement();

        if (player->isAlive()) {
            if (IsKeyDown(KEY_A)) {
                player->moveLeft();
            } else if (IsKeyDown(KEY_D)) {
                player->moveRight();
            }

            if (IsKeyPressed(KEY_W) && player->mIsCollidingBottom) {
                player->jump();
            }
        }
    }

    // Swap between levels
    if (IsKeyDown(KEY_ONE)) {
        gCurrentScene->getState().nextSceneId = SceneId::LEVEL_A;
    } else if (IsKeyDown(KEY_TWO)) {
        gCurrentScene->getState().nextSceneId = SceneId::LEVEL_B;
    } else if (IsKeyDown(KEY_THREE)) {
        gCurrentScene->getState().nextSceneId = SceneId::LEVEL_C;
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
        if (Map* map = gCurrentScene->getState().map) {
            clampCamera(&gCamera, map->getBounds());
        }

        deltaTime -= constants::FIXED_TIMESTEP;
    }

    gTimeAccumulator = deltaTime;
}

void render() {
    BeginDrawing();
    BeginMode2D(gCamera);

    gCurrentScene->render();

    EndMode2D();

    gCurrentScene->renderUI();

    EndDrawing();
}

void shutdown() {
    for (std::pair<const SceneId, Scene*>& entry : gLevels) {
        delete entry.second;
    }

    gLevels.clear();

    Entity::registry().clear();
    TextureAsset::registry().clear();

    Audio::get().shutdown();

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