#ifndef SLIME_H
#define SLIME_H

#include "entity.h"

class Slime: public Actor {
private:
    enum class Direction { LEFT, RIGHT };

public:
    enum class Type { WANDERER, CHASER, JUMPER, THROWER };
    enum class AIState { MOVING, IDLE, FOLLOWING, ATTACKING };
    enum class AnimationState { SPAWNING, ACTIVE };

    Type mType;
    AIState mAIState;
    bool mIsSpawning = true;
    float mSpawnedTime = 0.0f;
    Actor* mTarget = nullptr;

    float mPatrolOriginX = 0.0f;
    bool mHasPatrolOrigin = false;
    float mStateTimer = 0.0f;

    float mJumpCooldown = 0.0f;

    float mThrowCooldown = 0.0f;

    Direction mDirection = Direction::RIGHT;

private:
    bool shouldAnimate() const override;

    bool isBlocked() const;
    bool isLedgeAhead() const;
    void switchDirection();
    void move();
    void wander(float deltaTime);
    void follow(float deltaTime);
    void shoot(float deltaTime);
    void shootSlimeBall(Vector2 targetPosition);

public:
    Slime(Type type);

    void jump() override;
    void spawn(float spawnTime);
    void update(float deltaTime) override;
    void die();
};

class SlimeBall: public PhysicsEntity {
protected:
    void handleCollision(Collision collision) override;

public:
    SlimeBall(PhysicsEntity* target);

    void update(float deltaTime) override;
};

struct SlimeSpawn {
    Slime::Type type;
    int32_t col;
    int32_t row;
    int32_t delay;
};

#endif