#include "map_objects.h"

Coin::Coin() : PhysicsEntity(TextureAsset::get("coin")) {
    mIsSolid = false;
    mAnchored = true;

    this->multiplyScale(0.15f);
}