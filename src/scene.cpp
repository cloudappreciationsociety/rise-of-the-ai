#include "scene.h"

#include "constants.h"

Scene::Scene() : mGameState {}, mOrigin {constants::SCREEN_CENTER} {}

Scene::Scene(Vector2 origin, const char* bgHexCode) :
    mGameState {},
    mOrigin {origin},
    mBgColorHexCode {bgHexCode} {}

std::vector<PhysicsEntity*> Scene::getPlayerCollisionCandidates() const {
    std::vector<PhysicsEntity*> playerCollisionCandidates;
    for (Actor* enemy : mGameState.enemies) {
        playerCollisionCandidates.push_back(enemy);
    }
    for (PhysicsEntity* object : mGameState.objects) {
        playerCollisionCandidates.push_back(object);
    }

    return playerCollisionCandidates;
}

void Scene::initialize() {
    mGameState.nextSceneId = SceneId::NO_SCENE;
}

void Scene::shutdown() {
    if (mGameState.bg) {
        mGameState.bg->markForDeletion();
        mGameState.bg = nullptr;
    }

    mGameState.player = nullptr;
    mGameState.enemies.clear();
    mGameState.objects.clear();

    for (auto& pair : Entity::registry()) {
        if (PhysicsEntity* e = dynamic_cast<PhysicsEntity*>(pair.second.get())) {
            e->markForDeletion();
        }
    }

    delete mGameState.map;
    mGameState.map = nullptr;
}