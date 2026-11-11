#include "scene.h"

Scene::Scene() : mGameState {}, mOrigin {} {}

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
