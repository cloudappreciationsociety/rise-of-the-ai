#include "map.h"

Map::Map(
    int mapColumns,
    int mapRows,
    int32_t* levelData,
    float tileSize,
    const char* textureAlias,
    Vector2 origin
) :
    mMapColumns {mapColumns},
    mMapRows {mapRows},
    mLevelData {levelData},
    mTileSize {tileSize},
    mTexture {TextureAsset::get(textureAlias)},
    mOrigin {origin} {
    build();
}

Map::~Map() {}

void Map::build() {
    // Calculate map boundaries in world coordinates
    mLeftBoundary = mOrigin.x - (mMapColumns * mTileSize) / 2.0f;
    mRightBoundary = mOrigin.x + (mMapColumns * mTileSize) / 2.0f;
    mTopBoundary = mOrigin.y - (mMapRows * mTileSize) / 2.0f;
    mBottomBoundary = mOrigin.y + (mMapRows * mTileSize) / 2.0f;

    const TextureAsset* texture = mTexture.mAsset;

    int textureRows = texture->mSpriteSheetDimensions.x;
    int textureCols = texture->mSpriteSheetDimensions.y;

    // Precompute texture areas for each tile
    for (int row = 0; row < textureRows; row++) {
        for (int col = 0; col < textureCols; col++) {
            Rectangle textureArea = {
                static_cast<float>(col) * (texture->getWidth() / textureCols),
                static_cast<float>(row) * (texture->getHeight() / textureRows),
                static_cast<float>(texture->getWidth()) / textureCols,
                static_cast<float>(texture->getHeight()) / textureRows
            };

            mTextureAreas.push_back(textureArea);
        }
    }
}

void Map::render() {
    const Texture2D& sheet = mTexture.mAsset->getTexture2D();

    // Draw each tile in the map
    for (int row = 0; row < mMapRows; row++) {
        // Draw each column in the row
        for (int col = 0; col < mMapColumns; col++) {
            // Get the tile index at the current row and column
            int tile = mLevelData[row * mMapColumns + col];

            // If the tile index is -1, we do not draw anything
            if (tile == -1) {
                continue;
            }

            Rectangle destinationArea = {
                mLeftBoundary + col * mTileSize,
                mTopBoundary + row * mTileSize, // y-axis is inverted
                mTileSize,
                mTileSize
            };

            DrawTexturePro(sheet, mTextureAreas[tile], destinationArea, {0.0f, 0.0f}, 0.0f, WHITE);
        }
    }
}

bool Map::isSolidTileAt(Vector2 position, float* xOverlap, float* yOverlap) const {
    *xOverlap = 0.0f;
    *yOverlap = 0.0f;

    if (position.x < mLeftBoundary || position.x > mRightBoundary || position.y < mTopBoundary
        || position.y > mBottomBoundary) {
        return false;
    }

    int tileXIndex = floor((position.x - mLeftBoundary) / mTileSize);
    int tileYIndex = floor((position.y - mTopBoundary) / mTileSize);

    if (tileXIndex < 0 || tileXIndex >= mMapColumns || tileYIndex < 0 || tileYIndex >= mMapRows) {
        return false;
    }

    int tile = mLevelData[tileYIndex * mMapColumns + tileXIndex];
    if (tile == -1) {
        return false;
    }

    float tileCentreX = mLeftBoundary + tileXIndex * mTileSize + mTileSize / 2.0f;
    float tileCentreY = mTopBoundary + tileYIndex * mTileSize + mTileSize / 2.0f;

    /*
    When our collision probe touches a solid tile, we calculate how far the probe has penetrated into that tile along each axis — this is what we call the overlap. Each tile has a center point and a known half-size (half of mTileSize). By comparing the probe’s position to the tile’s center, we find how deep the probe is inside the tile: if the distance between them is smaller than the tile’s half-size, then the probe is overlapping. The overlap value (mTileSize / 2) - fabs(position - tileCenter) tells us exactly how much to push the entity back so that it sits flush against the tile’s edge without clipping inside. We calculate this separately for both the X and Y axes, which lets us resolve collisions in either direction—for example, preventing the player from sinking into the ground or walking through walls.
    */
    *xOverlap = fmaxf(0.0f, (mTileSize / 2.0f) - fabs(position.x - tileCentreX));
    *yOverlap = fmaxf(0.0f, (mTileSize / 2.0f) - fabs(position.y - tileCentreY));

    return true;
}