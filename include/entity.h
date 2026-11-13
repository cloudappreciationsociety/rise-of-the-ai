#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>
#include <memory>

#include "lib.h"
#include "texture.h"

enum class Direction { LEFT, UP, RIGHT, DOWN }; // For movement
enum class CollisionFace { TOP, BOTTOM, LEFT, RIGHT };

enum class EntityStatus { ACTIVE, INACTIVE };

class Entity {
private:
    static uint32_t idCounter;

protected:
    static constexpr int DEFAULT_SIZE = 250;

protected:
    bool mShouldDelete = false;

public:
    const uint32_t mId;
    Vector2 mPosition = {0.0f, 0.0f};
    Vector2 mScale = {Entity::DEFAULT_SIZE, Entity::DEFAULT_SIZE};
    float mAngle = 0.0f;

    TextureInstance mTexture;
    EntityStatus mStatus = EntityStatus::ACTIVE;

protected:
    virtual bool shouldAnimate() const {
        return true;
    };

public:
    // Returns a map of all currently active objects
    // std::map over std::unordered_map to guarantee rendering order so BG renders
    // behind everything else
    static std::map<uint32_t, std::unique_ptr<Entity>>& registry();
    static void updateAll(float deltaTime);
    static void renderAll();

    template<typename T>
    static T* create(T* object) {
        auto ptr = std::unique_ptr<T>(object);
        T* outPtr = ptr.get();
        Entity::registry().emplace(ptr->mId, std::move(ptr));
        return outPtr;
    }

    Entity();
    Entity(const TextureAsset* textureAsset);

    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    virtual void update(float deltaTime);
    virtual void render() const;

    virtual bool shouldUpdate() const {
        return mStatus == EntityStatus::ACTIVE;
    };

    const bool isMarkedForDeletion() const {
        return mShouldDelete;
    }

    void markForDeletion() {
        mShouldDelete = true;
    }

    void activate() {
        mStatus = EntityStatus::ACTIVE;
    }

    void deactivate() {
        mStatus = EntityStatus::INACTIVE;
    }

    bool isActive() {
        return mStatus == EntityStatus::ACTIVE;
    }

    virtual void multiplyScale(float scaleFactor) {
        mScale = Vector2Scale(mScale, scaleFactor);
    }
};

// Forward declaration
class Map;

// An entity that interacts with the world, physically
class PhysicsEntity: public Entity {
private:
    static constexpr float Y_COLLISION_THRESHOLD = 0.5f;

public:
    struct Collision {
        PhysicsEntity* entity;
        CollisionFace face;
    };

protected:
    std::vector<PhysicsEntity*> mCollisionCandidates;
    std::vector<Collision> mCollisions;

public:
    const Map* mMap = nullptr;

    bool mIsCollidingWithMap = false;
    bool mIsCollidingTop = false;
    bool mIsCollidingBottom = false;
    bool mIsCollidingRight = false;
    bool mIsCollidingLeft = false;
    bool mIsSolid = true;

    Vector2 mVelocity = {0.0f, 0.0f};
    Vector2 mAcceleration = {0.0f, 0.0f};
    Vector2 mColliderDimensions = {Entity::DEFAULT_SIZE, Entity::DEFAULT_SIZE};

    // If true, this object will not move in collideAndMove.
    bool mAnchored = false;

private:
    void resetColliderFlags();
    void checkSolidCollisionsX();
    void checkMapCollisionsX();
    void checkSolidCollisionsY();
    void checkMapCollisionsY();
    void checkNonsolidCollisions();

protected:
    void collideAndMove(float deltaTime);
    void applyDragX(float deltaTime, float dragCoefficient);

    void displayCollider() const;

    virtual bool canCollideWith(const PhysicsEntity* other) const {
        return other != this && other->mStatus == EntityStatus::ACTIVE;
    }

    virtual void handleCollision(Collision collision) {
        return;
    }

public:
    using Entity::Entity;

    bool isColliding(const PhysicsEntity* other) const;
    void setCollisionCandidates(const std::vector<PhysicsEntity*>& entities);
    void update(float deltaTime) override;
    void render() const override;

    bool isStationary() const {
        return Vector2Length(mVelocity) < 0.001f;
    }

    void multiplyScale(float scaleFactor) {
        Entity::multiplyScale(scaleFactor);
        mColliderDimensions = Vector2Scale(mColliderDimensions, scaleFactor);
    }
};

// An entity with health
class Actor: public PhysicsEntity {
private:
    static constexpr float KNOCKBACK_DRAG = 5.0f;

public:
    enum class MoveDirection { UP, DOWN, LEFT, RIGHT };

    float mHealth = 0.0f;
    float mMaxHealth = 0.0f;
    Vector2 mMovement = {0.0f, 0.0f};
    float mMoveSpeed = 100.0f;
    MoveDirection direction = MoveDirection::RIGHT;
    bool mIsJumping = false;
    float mJumpPower = 150.0f;

protected:
    float mStunTimer = 0.0f;
    void updateStun(float deltaTime);

public:
    using PhysicsEntity::PhysicsEntity;

    bool isStunned() const {
        return mStunTimer > 0.0f;
    }

    void knockback(Vector2 velocity, float stunSeconds) {
        mVelocity = velocity;
        mStunTimer = stunSeconds;
    }

    void applyMovement() {
        this->normalizeMovement();
        mVelocity.x = mMovement.x * mMoveSpeed;
    }

    void applyJump() {
        if (mIsJumping) {
            mIsJumping = false;
            mVelocity.y -= mJumpPower;
        }
    }

    virtual void jump() {
        mIsJumping = true;
    }

    bool isAlive() const {
        return mHealth > 0.0f;
    }

    virtual void takeDamage(float damage) {
        mHealth = std::max(0.0f, mHealth - damage);
    }

    virtual void takeDamage(float damage, Vector2 addVelocity) {
        this->takeDamage(damage);
        mVelocity = Vector2Add(mVelocity, addVelocity);
    }

    void resetMovement() {
        mMovement = {0.0f, 0.0f};
    }

    void moveUp() {
        mMovement.y = -1;
    }

    void moveDown() {
        mMovement.y = 1;
    }

    void moveLeft() {
        mMovement.x = -1;
    }

    void moveRight() {
        mMovement.x = 1;
    }

    void normalizeMovement() {
        if (Vector2Length(mMovement) > 0.0f) {
            mMovement = Vector2Normalize(mMovement);
        }
    }
};

#endif // ENTITY_H