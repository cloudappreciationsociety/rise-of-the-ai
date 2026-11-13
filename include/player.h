#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

class Player: public Actor {
private:
    static constexpr float INVULN_FRAME_TIME = 0.5f;

public:
    enum class AnimationState { IDLE, MOVING, JUMPING, DEAD };

    bool mStompedSlime = false;
    float mInvulnerabilityTimer = 0.0f;

private:
    bool isInvulnerable() const {
        return mInvulnerabilityTimer > 0.0f;
    }

protected:
    void handleCollision(Collision collision) override;

public:
    Player();

    void takeDamage(float damage) override;
    void jump() override;
    void update(float deltaTime) override;
};

#endif