#include "menus.h"

void StartMenu::initialize() {}

void StartMenu::update(float deltaTime) {
    if (IsKeyDown(KEY_ENTER)) {
        mGameState.nextSceneId = SceneId::LEVEL_A;
    }
}

void StartMenu::render() {}

void StartMenu::shutdown() {}

void WinMenu::initialize() {}

void WinMenu::update(float deltaTime) {
    if (IsKeyDown(KEY_ENTER)) {
        mGameState.nextSceneId = SceneId::START;
    }
}

void WinMenu::render() {}

void WinMenu::shutdown() {}

void LoseMenu::initialize() {}

void LoseMenu::update(float deltaTime) {
    if (IsKeyDown(KEY_ENTER)) {
        mGameState.nextSceneId = SceneId::START;
    }
}

void LoseMenu::render() {}

void LoseMenu::shutdown() {}