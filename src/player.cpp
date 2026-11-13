#include "player.h"

#include "audio.h"
#include "slime.h"

Player::Player() : Actor(TextureAsset::get("knight")) {
    mScale = {75.0f, 75.0f};
    mColliderDimensions = {mScale.x * 0.6f, mScale.y * 0.75f};
    mJumpPower = 500.0f;
    mMoveSpeed = 200.0f;
    mHealth = mMaxHealth = 3.0f;
    mIsSolid = false;
}

void Player::handleCollision(Collision collision) {
    static constexpr float STOMP_DEPTH_FRACTION = 0.7f; // Feet must be in the top 70% of the slime

    constexpr float PLAYER_KNOCKBACK_X = 220.0f;
    constexpr float PLAYER_KNOCKBACK_Y = 350.0f;
    constexpr float PLAYER_STUN_SECONDS = 0.35f;
    constexpr float SLIME_KNOCKBACK_X = 160.0f;
    constexpr float SLIME_KNOCKBACK_Y = 150.0f;
    constexpr float SLIME_STUN_SECONDS = 0.5f;

    auto entity = collision.entity;

    if (Slime* slime = dynamic_cast<Slime*>(entity)) {
        if (slime->mIsSpawning) {
            return;
        }

        float feetY = mPosition.y + mColliderDimensions.y / 2.0f;
        float slimeTopY = slime->mPosition.y - slime->mColliderDimensions.y / 2.0f;

        bool falling = mVelocity.y > 0.0f;
        bool feetOnTop = (feetY - slimeTopY) <= slime->mColliderDimensions.y * STOMP_DEPTH_FRACTION;

        // Stomped a slime: kill it and launch ourselves up
        if (falling && feetOnTop) {
            slime->die();
            mStompedSlime = true;
            return;
        }

        // Touched from the side or below: apply a push-back force
        if (!this->isInvulnerable()) {
            this->takeDamage(1.0f);

            float sign = (mPosition.x >= slime->mPosition.x) ? 1.0f : -1.0f;

            if (this->isAlive()) {
                this->knockback(
                    {sign * PLAYER_KNOCKBACK_X, -PLAYER_KNOCKBACK_Y},
                    PLAYER_STUN_SECONDS
                );
            }

            slime->knockback({-sign * SLIME_KNOCKBACK_X, -SLIME_KNOCKBACK_Y}, SLIME_STUN_SECONDS);
        }
    }
}

void Player::takeDamage(float damage) {
    if (!this->isAlive() || this->isInvulnerable()) {
        return;
    }

    Actor::takeDamage(damage);

    Audio::get().playSound(!this->isAlive() ? "knight_death" : "knight_hurt");
    mInvulnerabilityTimer = Player::INVULN_FRAME_TIME;
}

void Player::jump() {
    Actor::jump();

    Audio::get().playSound("knight_jump");
}

void Player::update(float deltaTime) {
    if (!this->shouldUpdate()) {
        return;
    }

    mStompedSlime = false;
    this->updateStun(deltaTime);

    if (mInvulnerabilityTimer > 0.0f) {
        mInvulnerabilityTimer = std::max(0.0f, mInvulnerabilityTimer - deltaTime);
    }

    // Blink effect while invulnerable
    bool isBlinking = mInvulnerabilityTimer > 0.0f
        && static_cast<int32_t>(mInvulnerabilityTimer * 12.0f) % 2 == 0;
    mTexture.mTint = isBlinking ? Fade(WHITE, 0.35f) : WHITE;

    if (!this->isStunned()) {
        this->applyMovement();

        if (mMovement.x < 0.0f) {
            mTexture.mIsFlippedX = true;
        } else if (mMovement.x > 0.0f) {
            mTexture.mIsFlippedX = false;
        }

        this->applyJump();
    }

    this->collideAndMove(deltaTime);

    static constexpr float STOMP_JUMP_VELOCITY = 550.0f;
    if (mStompedSlime) {
        mVelocity.y = -STOMP_JUMP_VELOCITY;
    }

    Player::AnimationState state = Player::AnimationState::IDLE;
    if (!mIsCollidingBottom) {
        state = Player::AnimationState::JUMPING;
    } else if (mMovement.x != 0.0f) {
        state = Player::AnimationState::MOVING;
    } else if (!this->isAlive()) {
        state = Player::AnimationState::DEAD;
    }
    mTexture.setAnimation(static_cast<int32_t>(state));

    if (this->shouldAnimate()) {
        mTexture.animate(deltaTime);
    }
}