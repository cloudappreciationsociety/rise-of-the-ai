#include "player.h"

#include "slime.h"

Player::Player() : Actor(TextureAsset::get("knight")) {
    mScale = {75.0f, 75.0f};
    mColliderDimensions = Vector2Scale(mScale, 0.75f);
    mJumpPower = 400.0f;
}

void Player::handleCollision(Collision collision) {
    auto entity = collision.entity;

    // Collided with a slime: die
    if (Slime* slime = dynamic_cast<Slime*>(entity)) {
        // If we stomp on a slime, they die
        if (collision.face == CollisionFace::BOTTOM) {
            slime->mHealth = 0.0f;
        } else {
            --mLives;
        }
    }
}

void Player::update(float deltaTime) {
    if (!this->shouldUpdate()) {
        return;
    }

    this->applyMovement();

    if (mMovement.x < 0.0f) {
        mTexture.mIsFlippedX = true;
    } else if (mMovement.x > 0.0f) {
        mTexture.mIsFlippedX = false;
    }

    this->applyJump();
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