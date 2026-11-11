#include "player.h"

Player::Player() : Actor(TextureAsset::get("knight")) {
    mScale = {75.0f, 75.0f};
    mColliderDimensions = Vector2Scale(mScale, 0.75f);
    mJumpPower = 400.0f;
}

void Player::update(float deltaTime) {
    if (!this->shouldUpdate()) {
        return;
    }

    this->normalizeMovement();
    mVelocity.x = mMovement.x * mMoveSpeed;

    if (mMovement.x < 0.0f) {
        mTexture.mIsFlippedX = true;
    } else if (mMovement.x > 0.0f) {
        mTexture.mIsFlippedX = false;
    }

    this->handleJump();
    this->collideAndMove(deltaTime);

    Player::AnimationState state = Player::AnimationState::IDLE;
    if (!mIsCollidingBottom) {
        state = Player::AnimationState::JUMPING;
    } else if (mMovement.x != 0.0f) {
        state = Player::AnimationState::MOVING;
    }
    mTexture.setAnimation(static_cast<int32_t>(state));

    if (this->shouldAnimate()) {
        mTexture.animate(deltaTime);
    }
}