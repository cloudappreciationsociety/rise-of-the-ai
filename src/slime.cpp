#include "slime.h"

#include "audio.h"
#include "map.h"

static float getRandomFloat(float low, float high) {
    return low + (high - low) * (static_cast<float>(GetRandomValue(0, 1000)) / 1000.0f);
}

Slime::Slime(Slime::Type type) : mType(type), mAIState(AIState::IDLE) {
    mScale = {50.0f, 50.0f};
    mColliderDimensions = {mScale.x, mScale.y};
    mIsSolid = false;

    switch (mType) {
        case (Slime::Type::WANDERER): {
            mMoveSpeed = getRandomFloat(75.0f, 150.0f);
            break;
        }
        case (Slime::Type::CHASER): {
            mJumpPower = getRandomFloat(450.0f, 500.0f);
            mMoveSpeed = getRandomFloat(150.0f, 175.0f);
            break;
        }
        case (Slime::Type::THROWER): {
            mTexture.mTint = ORANGE;
            mMoveSpeed = getRandomFloat(50.0f, 125.0f);
            break;
        }

        default:
            break;
    }
}

bool Slime::shouldAnimate() const {
    return true;
}

void Slime::jump() {
    Actor::jump();

    Audio::get().playSound("slime_jump");
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

// Check if there's a drop in the direction we're moving
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
    static constexpr float PATROL_RADIUS =
        225.0f; // Maximum horizontal distance this can patrol, both directions (3 tiles)
    static constexpr float PATROL_TIME_MIN = 1.5f; //
    static constexpr float PATROL_TIME_MAX = 3.0f;
    static constexpr float PAUSE_MIN = 0.5f;
    static constexpr float PAUSE_MAX = 1.5f;

    // Establish center of patrol
    if (!mHasPatrolOrigin) {
        mHasPatrolOrigin = true;
        mPatrolOriginX = mPosition.x;
        mDirection = (GetRandomValue(0, 1) == 0) ? Direction::LEFT : Direction::RIGHT;
        mStateTimer = getRandomFloat(0.0f, PAUSE_MAX);
    }

    mStateTimer -= deltaTime;

    switch (mAIState) {
        case (AIState::IDLE): {
            if (mStateTimer > 0.0f) {
                break;
            }

            // Turn around if we've wandered too far off
            if (mPosition.x < mPatrolOriginX - PATROL_RADIUS) {
                mDirection = Direction::RIGHT;
            } else if (mPosition.x > mPatrolOriginX + PATROL_RADIUS) {
                mDirection = Direction::LEFT;
            } else if (GetRandomValue(0, 3) == 0) { // 1/3 chance to switch directions
                this->switchDirection();
            }

            mAIState = AIState::MOVING;
            mStateTimer = getRandomFloat(PATROL_TIME_MIN, PATROL_TIME_MAX);
            break;
        }

        case (AIState::MOVING): {
            bool atPatrolEdge =
                (mDirection == Direction::LEFT && mPosition.x <= mPatrolOriginX - PATROL_RADIUS)
                || (mDirection == Direction::RIGHT
                    && mPosition.x >= mPatrolOriginX + PATROL_RADIUS);

            // Once we've reached the edge of the patrol, turn around and idle for a bit
            if (this->isBlocked() || this->isLedgeAhead() || atPatrolEdge) {
                this->switchDirection();
                mAIState = AIState::IDLE;
                mStateTimer = getRandomFloat(PAUSE_MIN, PAUSE_MAX);
            } else if (mStateTimer <= 0.0f) { // Idle if we finished moving
                mAIState = AIState::IDLE;
                mStateTimer = getRandomFloat(PAUSE_MIN, PAUSE_MAX);
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
    static constexpr float AGGRO_RANGE = 500.0f;
    static constexpr float DEAGGRO_RANGE = 750.0f;
    static constexpr float STOP_DISTANCE = 12.0f; // Don't jitter over the player
    static constexpr float LEDGE_DROP_ALLOWED =
        50.0f; // Only walk off a ledge if the player is this far below
    static constexpr float JUMP_Y_THRESHOLD = 60.0f; // Player this far above counts as "above"
    static constexpr float JUMP_X_THRESHOLD =
        300.0f; // Must also be within this horizontal distance to jump
    static constexpr float JUMP_COOLDOWN = 1.2f;

    if (mTarget == nullptr) {
        return;
    }

    mJumpCooldown = std::max(0.0f, mJumpCooldown - deltaTime);

    Vector2 toTarget = Vector2Subtract(mTarget->mPosition, mPosition);
    float distance = Vector2Length(toTarget);

    switch (mAIState) {
        case (AIState::IDLE): {
            if (distance < AGGRO_RANGE) {
                mAIState = AIState::MOVING;
            }

            break;
        }

        case (AIState::MOVING): {
            if (distance > DEAGGRO_RANGE) {
                mAIState = AIState::IDLE;
                break;
            }

            Direction direction = (toTarget.x < 0.0f) ? Direction::LEFT : Direction::RIGHT;

            bool closeEnough = std::fabs(toTarget.x) < STOP_DISTANCE;

            // Don't run off a ledge unless the player is down there
            bool holdAtLedge = this->isLedgeAhead() && toTarget.y < LEDGE_DROP_ALLOWED;

            bool grounded = mIsCollidingBottom;

            if (!closeEnough && !holdAtLedge || !mIsCollidingBottom) {
                mDirection = direction;
                this->move();
            }

            bool blockedByWall = !closeEnough && this->isBlocked();
            bool shouldJump =
                toTarget.y < -JUMP_Y_THRESHOLD && std::fabs(toTarget.x) < JUMP_X_THRESHOLD;

            if (grounded && mJumpCooldown <= 0.0f && (blockedByWall || shouldJump)) {
                this->jump();
                mJumpCooldown = JUMP_COOLDOWN;
            }

            break;
        }

        default:
            break;
    }
}

void Slime::shoot(float deltaTime) {
    static constexpr float MIN_RANGE = 200.0f; // Closer than this: back away
    static constexpr float MAX_RANGE = 500.0f; // Farther than this: ignore the player
    static constexpr float WINDUP = 0.8f; // Delay before the first throw
    static constexpr float THROW_COOLDOWN = 2.0f;
    static constexpr float TARGET_DEVIATION = 10.0f;

    if (mTarget == nullptr) {
        return;
    }

    mThrowCooldown = std::max(0.0f, mThrowCooldown - deltaTime);

    // Add some randomness to the aim
    Vector2 targetPosition = Vector2Add(
        mTarget->mPosition,
        {getRandomFloat(-TARGET_DEVIATION, TARGET_DEVIATION),
         getRandomFloat(-TARGET_DEVIATION, TARGET_DEVIATION)}
    );

    Vector2 toTarget = Vector2Subtract(targetPosition, mPosition);
    float distance = Vector2Length(toTarget);

    if (distance > MAX_RANGE) {
        // Re-arm the wind-up so it doesn't fire the instant the player walks in range
        mAIState = AIState::IDLE;
        mThrowCooldown = std::max(mThrowCooldown, WINDUP);
        return;
    }

    mAIState = AIState::MOVING;

    // Always face the player
    mTexture.mIsFlippedX = toTarget.x < 0.0f;

    // Move the opposite direction of the player
    mDirection = (toTarget.x < 0.0f) ? Direction::RIGHT : Direction::LEFT;

    if (distance < MIN_RANGE && !this->isBlocked() && !this->isLedgeAhead()) {
        this->move();
    }

    if (mThrowCooldown <= 0.0f) {
        this->shootSlimeBall(targetPosition);
        mThrowCooldown = THROW_COOLDOWN;
    }
}

void Slime::shootSlimeBall(Vector2 targetPosition) {
    constexpr float BALL_HORIZONTAL_SPEED = 350.0f;
    constexpr float BALL_MIN_FLIGHT = 0.5f;
    static constexpr float BALL_GRAVITY = 600.0f;

    int32_t sign = (mTarget->mPosition.x - mPosition.x < 0.0f) ? -1 : 1;
    Vector2 origin = {mPosition.x + sign * (mScale.x / 2.0f), mPosition.y - mScale.y / 4.0f};

    Vector2 delta = Vector2Subtract(targetPosition, origin);

    // t = x / v_x
    float flightTime = std::max(std::fabs(delta.x) / BALL_HORIZONTAL_SPEED, BALL_MIN_FLIGHT);

    // Vertical launch velocity: v = delta_y / t - 0.5 * g * t
    Vector2 velocity = {
        sign * BALL_HORIZONTAL_SPEED,
        (delta.y / flightTime - 0.5f * BALL_GRAVITY * flightTime)
    };

    Audio::get().playSound("slime_throw");
    SlimeBall* projectile = Entity::create(new SlimeBall(mTarget));
    projectile->mPosition = origin;
    projectile->mVelocity = velocity;
    projectile->mAcceleration = {0.0f, BALL_GRAVITY};
    projectile->mMap = mMap;
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

    this->updateStun(deltaTime);

    this->resetMovement();

    if (!mIsSpawning && !this->isStunned()) {
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
                this->shoot(deltaTime);
                break;
            }

            default:
                break;
        }
    }

    if (!this->isStunned()) {
        this->applyMovement();

        if (mType != Slime::Type::THROWER) {
            if (mMovement.x < 0.0f) {
                mTexture.mIsFlippedX = true;
            } else if (mMovement.x > 0.0f) {
                mTexture.mIsFlippedX = false;
            }
        }

        this->applyJump();
    }

    this->collideAndMove(deltaTime);

    if (this->shouldAnimate()) {
        mTexture.animate(deltaTime);
    }
}

void Slime::die() {
    Audio::get().playSound("slime_death");

    this->deactivate();
}

SlimeBall::SlimeBall(PhysicsEntity* target) : PhysicsEntity(TextureAsset::get("slime_ball")) {
    mTexture.mTint = ORANGE;

    mScale = {15.0f, 15.0f};
    mColliderDimensions = mScale;
    this->setCollisionCandidates({target});
}

void SlimeBall::handleCollision(Collision collision) {
    if (Actor* actor = dynamic_cast<Actor*>(collision.entity)) {
        actor->takeDamage(1.0f);

        Audio::get().playSound("slime_ball_impact");
        this->markForDeletion();
    }
}

void SlimeBall::update(float deltaTime) {
    if (!this->shouldUpdate()) {
        return;
    }

    this->collideAndMove(deltaTime);

    if (mIsCollidingWithMap) {
        Audio::get().playSound("slime_ball_impact");
        this->markForDeletion();
    }
}
