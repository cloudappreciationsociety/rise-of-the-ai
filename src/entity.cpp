#include "entity.h"

#include "constants.h"
#include "map.h"

uint32_t Entity::idCounter = 0;

std::map<uint32_t, std::unique_ptr<Entity>>& Entity::registry() {
    static std::map<uint32_t, std::unique_ptr<Entity>> registry;
    return registry;
}

void Entity::updateAll(float deltaTime) {
    for (auto& pair : Entity::registry()) {
        pair.second->update(deltaTime);
    }
}

void Entity::renderAll() {
    for (const auto& pair : Entity::registry()) {
        pair.second->render();
    }
}

Entity::Entity() : mId(Entity::idCounter++) {}

Entity::Entity(const TextureAsset* textureAsset) :
    mId(Entity::idCounter++),
    mTexture(textureAsset) {}

void Entity::update(float deltaTime) {
    if (!this->shouldUpdate()) {
        return;
    }

    if (this->shouldAnimate()) {
        mTexture.animate(deltaTime);
    }
}

void Entity::render() const {
    if (mStatus == EntityStatus::INACTIVE) {
        return;
    }

    // Destination rectangle – centred on position
    Rectangle destinationArea =
        {mPosition.x, mPosition.y, static_cast<float>(mScale.x), static_cast<float>(mScale.y)};

    // Origin inside the source texture (centre of the texture)
    Vector2 originOffset = {
        static_cast<float>(mScale.x) / 2.0f,
        static_cast<float>(mScale.y) / 2.0f
    };

    // Render the texture on screen
    mTexture.render(destinationArea, originOffset, mAngle);
}

void PhysicsEntity::resetColliderFlags() {
    mIsCollidingWithMap = false;
    mIsCollidingTop = false;
    mIsCollidingBottom = false;
    mIsCollidingRight = false;
    mIsCollidingLeft = false;
}

/**
 * Checks if two entities are colliding based on their positions and collider
 * dimensions.
 *
 * @param other represents another Entity with which you want to check for
 * collision. It is a pointer to the Entity class.
 *
 * @return returns `true` if the two entities are colliding based on their
 * positions and collider dimensions, and `false` otherwise.
 */
bool PhysicsEntity::isColliding(const PhysicsEntity* other) const {
    if (!this->canCollideWith(other)) {
        return false;
    }

    float xDistance = fabs(mPosition.x - other->mPosition.x)
        - ((mColliderDimensions.x + other->mColliderDimensions.x) / 2.0f);
    float yDistance = fabs(mPosition.y - other->mPosition.y)
        - ((mColliderDimensions.y + other->mColliderDimensions.y) / 2.0f);

    return xDistance < 0.0f && yDistance < 0.0f;
}

void PhysicsEntity::checkSolidCollisionsX() {
    for (PhysicsEntity* collidableEntity : mCollisionCandidates) {
        if (!this->isColliding(collidableEntity)) {
            continue;
        }

        if (!collidableEntity->mIsSolid) {
            continue;
        }

        // When standing on a platform, we're always slightly overlapping
        // it vertically due to gravity, which causes false horizontal
        // collision detections. So the solution I found is only resolve X
        // collisions if there's significant Y overlap, preventing the
        // platform we're standing on from acting like a wall.
        float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
        float yOverlap = fabs(
            yDistance - (mColliderDimensions.y / 2.0f)
            - (collidableEntity->mColliderDimensions.y / 2.0f)
        );

        // Skip if barely touching vertically (standing on platform)
        if (yOverlap < PhysicsEntity::Y_COLLISION_THRESHOLD) {
            continue;
        }

        float xDistance = fabs(mPosition.x - collidableEntity->mPosition.x);
        float xOverlap = fabs(
            xDistance - (mColliderDimensions.x / 2.0f)
            - (collidableEntity->mColliderDimensions.x / 2.0f)
        );

        if (mVelocity.x > 0) {
            mPosition.x -= xOverlap;
            mVelocity.x = 0;

            // Collision!
            mIsCollidingRight = true;

            mCollisions.push_back({collidableEntity, CollisionFace::RIGHT});
        } else if (mVelocity.x < 0) {
            mPosition.x += xOverlap;
            mVelocity.x = 0;

            // Collision!
            mIsCollidingLeft = true;

            mCollisions.push_back({collidableEntity, CollisionFace::LEFT});
        }
    }
}

/**
 * Iterates through a list of collidable entities, checks for collisions with
 * the player entity, and resolves any vertical overlap by adjusting the
 * player's position and velocity accordingly.
 *
 * @param collidableEntities An array of pointers to `Entity` objects that
 * represent the entities that the current `Entity` instance can potentially
 * collide with. The `collisionCheckCount` parameter specifies the number of
 * entities in the `collidableEntities` array that need to be checked for
 * collision.
 * @param collisionCheckCount The number of entities that the current entity
 * (`Entity`) should check for collisions with. This parameter specifies how
 * many entities are in the `collidableEntities` array that need to be checked
 * for collisions with the current entity.
 */
void PhysicsEntity::checkSolidCollisionsY() {
    // STEP 1: For every entity that our player can collide with...
    for (PhysicsEntity* collidableEntity : mCollisionCandidates) {
        if (!this->isColliding(collidableEntity)) {
            continue;
        }

        if (!collidableEntity->mIsSolid) {
            continue;
        }

        // STEP 2: Calculate the distance between its centre and our centre
        //         and use that to calculate the amount of overlap between
        //         both bodies.
        float yDistance = fabs(mPosition.y - collidableEntity->mPosition.y);
        float yOverlap = fabs(
            yDistance - (mColliderDimensions.y / 2.0f)
            - (collidableEntity->mColliderDimensions.y / 2.0f)
        );

        // STEP 3: "Unclip" ourselves from the other entity, and zero our
        //         vertical velocity.
        if (mVelocity.y > 0) {
            mPosition.y -= yOverlap;
            mVelocity.y = 0;

            mIsCollidingBottom = true;

            mCollisions.push_back({collidableEntity, CollisionFace::BOTTOM});
        } else if (mVelocity.y < 0) {
            mPosition.y += yOverlap;
            mVelocity.y = 0;

            mIsCollidingTop = true;

            mCollisions.push_back({collidableEntity, CollisionFace::TOP});
        }
    }
}

void PhysicsEntity::checkMapCollisionsY() {
    if (mMap == nullptr)
        return;

    Vector2 topCentreProbe = {mPosition.x, mPosition.y - (mColliderDimensions.y / 2.0f)};
    Vector2 topLeftProbe = {
        mPosition.x - (mColliderDimensions.x / 2.0f),
        mPosition.y - (mColliderDimensions.y / 2.0f)
    };
    Vector2 topRightProbe = {
        mPosition.x + (mColliderDimensions.x / 2.0f),
        mPosition.y - (mColliderDimensions.y / 2.0f)
    };

    Vector2 bottomCentreProbe = {mPosition.x, mPosition.y + (mColliderDimensions.y / 2.0f)};
    Vector2 bottomLeftProbe = {
        mPosition.x - (mColliderDimensions.x / 2.0f),
        mPosition.y + (mColliderDimensions.y / 2.0f)
    };
    Vector2 bottomRightProbe = {
        mPosition.x + (mColliderDimensions.x / 2.0f),
        mPosition.y + (mColliderDimensions.y / 2.0f)
    };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ABOVE (jumping upward)
    if ((mMap->isSolidTileAt(topCentreProbe, &xOverlap, &yOverlap)
         || mMap->isSolidTileAt(topLeftProbe, &xOverlap, &yOverlap)
         || mMap->isSolidTileAt(topRightProbe, &xOverlap, &yOverlap))
        && mVelocity.y < 0.0f) {
        mPosition.y += yOverlap; // push down
        mVelocity.y = 0.0f;
        mIsCollidingTop = true;
        mIsCollidingWithMap = true;
    }

    // COLLISION BELOW (falling downward)
    if ((mMap->isSolidTileAt(bottomCentreProbe, &xOverlap, &yOverlap)
         || mMap->isSolidTileAt(bottomLeftProbe, &xOverlap, &yOverlap)
         || mMap->isSolidTileAt(bottomRightProbe, &xOverlap, &yOverlap))
        && mVelocity.y > 0.0f) {
        mPosition.y -= yOverlap; // push up
        mVelocity.y = 0.0f;
        mIsCollidingBottom = true;
        mIsCollidingWithMap = true;
    }
}

void PhysicsEntity::checkMapCollisionsX() {
    if (mMap == nullptr)
        return;

    Vector2 leftCentreProbe = {mPosition.x - (mColliderDimensions.x / 2.0f), mPosition.y};
    Vector2 leftTopProbe = {
        mPosition.x - (mColliderDimensions.x / 2.0f),
        mPosition.y - (mColliderDimensions.y / 2.0f)
    };
    Vector2 leftBottomProbe = {
        mPosition.x - (mColliderDimensions.x / 2.0f),
        mPosition.y + (mColliderDimensions.y / 2.0f)
    };

    Vector2 rightCentreProbe = {mPosition.x + (mColliderDimensions.x / 2.0f), mPosition.y};
    Vector2 rightTopProbe = {
        mPosition.x + (mColliderDimensions.x / 2.0f),
        mPosition.y - (mColliderDimensions.y / 2.0f)
    };
    Vector2 rightBottomProbe = {
        mPosition.x + (mColliderDimensions.x / 2.0f),
        mPosition.y + (mColliderDimensions.y / 2.0f)
    };

    float xOverlap = 0.0f;
    float yOverlap = 0.0f;

    // COLLISION ON RIGHT (moving right)
    if ((mMap->isSolidTileAt(rightCentreProbe, &xOverlap, &yOverlap)
         || mMap->isSolidTileAt(rightTopProbe, &xOverlap, &yOverlap)
         || mMap->isSolidTileAt(rightBottomProbe, &xOverlap, &yOverlap))
        && mVelocity.x > 0.0f && yOverlap >= 0.5f) {
        mPosition.x -= xOverlap * 1.01f; // push left
        mVelocity.x = 0.0f;
        mIsCollidingRight = true;
        mIsCollidingWithMap = true;
    }

    // COLLISION ON LEFT (moving left)
    if ((mMap->isSolidTileAt(leftCentreProbe, &xOverlap, &yOverlap)
         || mMap->isSolidTileAt(leftTopProbe, &xOverlap, &yOverlap)
         || mMap->isSolidTileAt(leftBottomProbe, &xOverlap, &yOverlap))
        && mVelocity.x < 0.0f && yOverlap >= 0.5f) {
        mPosition.x += xOverlap * 1.01f; // push right
        mVelocity.x = 0.0f;
        mIsCollidingLeft = true;
        mIsCollidingWithMap = true;
    }
}

void PhysicsEntity::checkNonsolidCollisions() {
    for (PhysicsEntity* other : mCollisionCandidates) {
        if (other->mIsSolid || !this->isColliding(other)) {
            continue;
        }

        float dx = other->mPosition.x - mPosition.x;
        float dy = other->mPosition.y - mPosition.y;

        float xPenetration =
            (mColliderDimensions.x + other->mColliderDimensions.x) / 2.0f - fabs(dx);
        float yPenetration =
            (mColliderDimensions.y + other->mColliderDimensions.y) / 2.0f - fabs(dy);

        CollisionFace face;
        if (xPenetration < yPenetration) {
            face = (dx > 0.0f) ? CollisionFace::RIGHT : CollisionFace::LEFT;
        } else {
            face = (dy > 0.0f) ? CollisionFace::BOTTOM : CollisionFace::TOP;
        }

        mCollisions.push_back({other, face});
    }
}

void PhysicsEntity::collideAndMove(float deltaTime) {
    if (mAnchored) {
        return;
    }

    this->resetColliderFlags();
    mCollisions.clear();

    mVelocity.x += mAcceleration.x * deltaTime;
    mVelocity.y += mAcceleration.y * deltaTime;

    mPosition.y += mVelocity.y * deltaTime;
    this->checkSolidCollisionsY();
    this->checkMapCollisionsY();

    mPosition.x += mVelocity.x * deltaTime;
    this->checkSolidCollisionsX();
    this->checkMapCollisionsX();

    this->checkNonsolidCollisions();

    for (const auto& collision : mCollisions) {
        this->handleCollision(collision);
    }
}

void PhysicsEntity::applyDragX(float deltaTime, float dragCoefficient) {
    float dragForce = -dragCoefficient * mVelocity.x;

    // If one step of drag would reverse the velocity, set it exactly to zero instead
    if (std::fabs(dragForce * deltaTime) > std::fabs(mVelocity.x)) {
        dragForce = -mVelocity.x / deltaTime;
    }

    mAcceleration.x = dragForce;
}

void PhysicsEntity::displayCollider() const {
    // draw the collision box
    Rectangle colliderBox = {
        mPosition.x - mColliderDimensions.x / 2.0f,
        mPosition.y - mColliderDimensions.y / 2.0f,
        mColliderDimensions.x,
        mColliderDimensions.y
    };

    DrawRectangleLines(
        colliderBox.x, // Top-left X
        colliderBox.y, // Top-left Y
        colliderBox.width, // Width
        colliderBox.height, // Height
        GREEN // Color
    );
}

void PhysicsEntity::setCollisionCandidates(const std::vector<PhysicsEntity*>& entities) {
    mCollisionCandidates.clear();

    for (auto e : entities) {
        mCollisionCandidates.push_back(e);
    }
}

void PhysicsEntity::update(float deltaTime) {
    if (!this->shouldUpdate()) {
        return;
    }

    this->collideAndMove(deltaTime);

    if (this->shouldAnimate()) {
        mTexture.animate(deltaTime);
    }
}

void PhysicsEntity::render() const {
    Entity::render();

    //this->displayCollider();
}

void Actor::updateStun(float deltaTime) {
    mStunTimer = std::max(0.0f, mStunTimer - deltaTime);

    if (mStunTimer > 0.0f) {
        mVelocity.x -= mVelocity.x * std::min(1.0f, KNOCKBACK_DRAG * deltaTime);
    }
}