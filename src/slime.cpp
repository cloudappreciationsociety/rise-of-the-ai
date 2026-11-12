#include "slime.h"

#include "map.h"

static float getRandomFloat(float low, float high) {
    return low + (high - low) * (static_cast<float>(GetRandomValue(0, 1000)) / 1000.0f);
}

Slime::Slime(Slime::Type type) : mType(type), mAIState(AIState::IDLE) {
    mScale = {50.0f, 50.0f};
    mColliderDimensions = {mScale.x, mScale.y};
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

// Have we run into a wall?
bool Slime::isBlocked() const {
    return (mDirection == Direction::LEFT && mIsCollidingLeft)
        || (mDirection == Direction::RIGHT && mIsCollidingRight);
}

bool Slime::isLedgeAhead() const {
    if (mMap == nullptr) {
        return false;
    }

    int32_t sign = (mDirection == Direction::LEFT) ? -1 : 1;

    Vector2 probe = {
        mPosition.x + sign * (mColliderDimensions.x / 2.0f + 4.0f),
        mPosition.y + mColliderDimensions.y / 2.0f + 8.0f
    };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;
    return !mMap->isSolidTileAt(probe, &xOverlap, &yOverlap);
}

void Slime::switchDirection() {
    mDirection = (mDirection == Direction::LEFT) ? Direction::RIGHT : Direction::LEFT;
}

void Slime::move() {
    if (mDirection == Direction::LEFT) {
        this->moveLeft();
    } else {
        this->moveRight();
    }
}

void Slime::wander(float deltaTime) {
    static constexpr float WANDER_PATROL_RADIUS =
        225.0f; // Maximum horizontal distance this can patrol, both directions (3 tiles)
    static constexpr float WANDER_PATROL_TIME_MIN = 1.5f; //
    static constexpr float WANDER_PATROL_TIME_MAX = 3.0f;
    static constexpr float WANDER_PAUSE_MIN = 0.5f;
    static constexpr float WANDER_PAUSE_MAX = 1.5f;

    // Establish center of patrol
    if (!mHasPatrolOrigin) {
        mHasPatrolOrigin = true;
        mPatrolOriginX = mPosition.x;
        mDirection = (GetRandomValue(0, 1) == 0) ? Direction::LEFT : Direction::RIGHT;
        mStateTimer = getRandomFloat(0.0f, WANDER_PAUSE_MAX);
    }

    mStateTimer -= deltaTime;

    switch (mAIState) {
        case (AIState::IDLE): {
            if (mStateTimer > 0.0f) {
                break;
            }

            // Turn around if we've wandered too far off
            if (mPosition.x < mPatrolOriginX - WANDER_PATROL_RADIUS) {
                mDirection = Direction::RIGHT;
            } else if (mPosition.x > mPatrolOriginX + WANDER_PATROL_RADIUS) {
                mDirection = Direction::LEFT;
            } else if (GetRandomValue(0, 3) == 0) { // 1/3 chance to switch directions
                this->switchDirection();
            }

            mAIState = AIState::MOVING;
            mStateTimer = getRandomFloat(WANDER_PATROL_TIME_MIN, WANDER_PATROL_TIME_MAX);
            break;
        }

        case (AIState::MOVING): {
            bool atPatrolEdge = (mDirection == Direction::LEFT
                                 && mPosition.x <= mPatrolOriginX - WANDER_PATROL_RADIUS)
                || (mDirection == Direction::RIGHT
                    && mPosition.x >= mPatrolOriginX + WANDER_PATROL_RADIUS);

            // Once we've reached the edge of the patrol, turn around and idle for a bit
            if (this->isBlocked() || this->isLedgeAhead() || atPatrolEdge) {
                this->switchDirection();
                mAIState = AIState::IDLE;
                mStateTimer = getRandomFloat(WANDER_PAUSE_MIN, WANDER_PAUSE_MAX);
            } else if (mStateTimer <= 0.0f) { // Idle if we finished moving
                mAIState = AIState::IDLE;
                mStateTimer = getRandomFloat(WANDER_PAUSE_MIN, WANDER_PAUSE_MAX);
            } else {
                this->move();
            }

            break;
        }

        default:
            break;
    }
}

void Slime::follow(float deltaTime) {
    switch (mAIState) {
        case (AIState::IDLE): {
            // If the player approaches...
            if (Vector2Distance(mPosition, mTarget->mPosition) < 250.0f) {
                // Switch to walking
                mAIState = AIState::MOVING;
            }

            break;
        }

        case (AIState::MOVING): {
            // Depending on where the player is in respect to their x-position
            // change direction of the NPC
            if (mPosition.x > mTarget->mPosition.x) {
                this->moveLeft();
            } else {
                this->moveRight();
            }
            break;
        }

        default:
            break;
    }
}

void Slime::update(float deltaTime) {
    if (!this->shouldUpdate()) {
        return;
    }

    float now = static_cast<float>(GetTime());
    if (mIsSpawning && now > mSpawnedTime) {
        mIsSpawning = false;
        mTexture.setAnimation(static_cast<int32_t>(Slime::AnimationState::ACTIVE));
        mTexture.setAnimationDuration(0.5f);
    }

    this->resetMovement();

    if (!mIsSpawning) {
        switch (mType) {
            case (Slime::Type::WANDERER): {
                this->wander(deltaTime);
                break;
            }

            case (Slime::Type::CHASER): {
                this->follow(deltaTime);
                break;
            }

            case (Slime::Type::THROWER): {
                break;
            }

            default:
                break;
        }
    }

    this->applyMovement();

    if (mMovement.x < 0.0f) {
        mTexture.mIsFlippedX = true;
    } else if (mMovement.x > 0.0f) {
        mTexture.mIsFlippedX = false;
    }

    this->applyJump();
    this->collideAndMove(deltaTime);

    if (this->shouldAnimate()) {
        mTexture.animate(deltaTime);
    }
}