#ifndef SLIME_H
#define SLIME_H

#include "entity.h"

class Slime: public Actor {
public:
    enum class Type { WANDERER, CHASER, JUMPER, THROWER };
    enum class AIState { MOVING, IDLE, FOLLOWING, ATTACKING };
    enum class AnimationState { SPAWNING, ACTIVE };

    Type mType;
    bool mIsSpawning = true;
    float mSpawnedTime = 0.0f;
    const Actor* mTarget = nullptr;

private:
    bool shouldAnimate() const override;

public:
    Slime(Type type);

    void spawn(float spawnTime);
    void update(float deltaTime) override;
};

#endif