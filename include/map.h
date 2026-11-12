#ifndef MAP_H
#define MAP_H

#include "texture.h"

class Map {
private:
    int mMapColumns; // number of columns in map
    int mMapRows; // number of rows in map

    int32_t* mLevelData; // array of tile indices
    TextureInstance mTexture;

    float mTileSize; // size of each tile in pixels

    std::vector<Rectangle> mTextureAreas; // texture areas for each tile
    Vector2 mOrigin; // center of the map in world coordinates

    float mLeftBoundary; // left boundary of the map in world coordinates
    float mRightBoundary; // right boundary of the map in world coordinates
    float mTopBoundary; // top boundary of the map in world coordinates
    float mBottomBoundary; // bottom boundary of the map in world coordinates

public:
    Map(int mapColumns,
        int mapRows,
        int32_t* levelData,
        float tileSize,
        const char* textureAlias,
        Vector2 origin);
    ~Map();

    void build();
    void render();
    bool isSolidTileAt(Vector2 position, float* xOverlap, float* yOverlap) const;

    Vector2 getCenterOfTile(int32_t col, int32_t row) const {
        return {
            mOrigin.x + (col + 0.5f - mMapColumns / 2.0f) * mTileSize,
            mOrigin.y + (row + 0.5f - mMapRows / 2.0f) * mTileSize
        };
    }

    int getMapColumns() const {
        return mMapColumns;
    }

    int getMapRows() const {
        return mMapRows;
    }

    float getTileSize() const {
        return mTileSize;
    }

    int32_t* getLevelData() const {
        return mLevelData;
    }

    float getLeftBoundary() const {
        return mLeftBoundary;
    }

    float getRightBoundary() const {
        return mRightBoundary;
    }

    float getTopBoundary() const {
        return mTopBoundary;
    }

    float getBottomBoundary() const {
        return mBottomBoundary;
    }
};

#endif // MAP_H