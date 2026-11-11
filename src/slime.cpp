#include "slime.h"

Slime::Slime(Slime::Type type) : mType(type) {
    mScale = {50.0f, 50.0f};
    mColliderDimensions = {mScale.x, mScale.y * 0.75f};
}

bool Slime::shouldAnimate() const {
    return true;
}

void Slime::spawn(float spawnTime) {
    mTexture.setAnimation(static_cast<int32_t>(Slime::AnimationState::SPAWNING));
    mTexture.setAnimationDuration(spawnTime);
    mIsSpawning = true;
    mSpawnedTime = static_cast<float>(GetTime()) + spawnTime;
}

void Slime::update(float deltaTime) {
    if (!this->shouldUpdate()) {
        return;
    }

    float now = static_cast<float>(GetTime());
    if (now > mSpawnedTime) {
        mIsSpawning = false;
        mTexture.setAnimation(static_cast<int32_t>(Slime::AnimationState::ACTIVE));
    }

    switch (mType) {
        case (Slime::Type::WANDERER): {
            break;
        }

        case (Slime::Type::CHASER): {
            break;
        }

        default:
            break;
    }

    this->collideAndMove(deltaTime);

    if (this->shouldAnimate()) {
        mTexture.animate(deltaTime);
    }
}