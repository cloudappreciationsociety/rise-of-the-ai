#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

class Player: public Actor {
public:
    enum class AnimationState { IDLE, MOVING, JUMPING, DEAD };

    float mMoveSpeed = 125.0f;
    uint32_t mLives = 3;

public:
    Player();

    void update(float deltaTime) override;
};

#endif