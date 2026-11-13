#include "menus.h"

#include "audio.h"
#include "constants.h"
#include "ui.h"

void StartMenu::initialize() {
    Scene::initialize();
    mBgColorHexCode = "#13fc03";

    Audio::get().playMusic("start_menu");
}

void StartMenu::update(float deltaTime) {
    Audio::get().update();

    if (IsKeyPressed(KEY_ENTER)) {
        mGameState.nextSceneId = SceneId::LEVEL_A;
    }
}

void StartMenu::render() {
    ClearBackground(ColorFromHex(mBgColorHexCode));

    drawText("Slime Killer", constants::SCREEN_CENTER, constants::ui::HEADER_FONT_SIZE, WHITE);
    drawText(
        "Press [ENTER] to start.",
        Vector2Add(constants::SCREEN_CENTER, {0.0f, 100.0f}),
        constants::ui::FONT_SIZE,
        WHITE
    );
}

void StartMenu::shutdown() {
    Audio::get().stopActiveMusic();
}

void WinMenu::initialize() {
    Scene::initialize();
    mBgColorHexCode = "#fca903";

    Audio::get().playSound("win");
}

void WinMenu::update(float deltaTime) {
    if (IsKeyPressed(KEY_ENTER)) {
        mGameState.nextSceneId = SceneId::START;
    }
}

void WinMenu::render() {
    ClearBackground(ColorFromHex(mBgColorHexCode));

    drawText("You WON!!!", constants::SCREEN_CENTER, constants::ui::HEADER_FONT_SIZE, WHITE);
    drawText(
        "Press [ENTER] to do it all over again. Press [Q] to quit.",
        Vector2Add(constants::SCREEN_CENTER, {0.0f, 100.0f}),
        constants::ui::FONT_SIZE,
        WHITE
    );
}

void WinMenu::shutdown() {}

void LoseMenu::initialize() {
    Scene::initialize();
    mBgColorHexCode = "#fc0303";

    Audio::get().playSound("lose");
}

void LoseMenu::update(float deltaTime) {
    if (IsKeyPressed(KEY_ENTER)) {
        mGameState.nextSceneId = SceneId::START;
    }
}

void LoseMenu::render() {
    ClearBackground(ColorFromHex(mBgColorHexCode));

    drawText("You died...", constants::SCREEN_CENTER, constants::ui::HEADER_FONT_SIZE, WHITE);
    drawText(
        "Press [ENTER] to restart. Press [Q] to quit.",
        Vector2Add(constants::SCREEN_CENTER, {0.0f, 100.0f}),
        constants::ui::FONT_SIZE,
        WHITE
    );
}

void LoseMenu::shutdown() {}