#include "texture.h"

/**
 * @brief Calculates and returns the UV coordinates and dimensions of a
 * rectangle slice from a texture based on the given index, number of rows, and
 * number of columns.
 *
 * @param texture a pointer to a `Texture2D` struct, contains information about
 * a 2D texture such as its width and height.
 * @param index represents the index of the specific slice within a texture
 * atlas. Each slice is a sub-image within the texture atlas that contains
 * multiple images arranged in rows and columns.
 * @param rows represents the number of rows in which the texture is divided.
 * This parameter is used to calculate the vertical position of the texture
 * slice based on the index provided.
 * @param cols represents the number of columns in a grid layout. It is used in
 * the `getUVRectangle` function to calculate the UV coordinates for a specific
 * index within the grid.
 *
 * @return a `Rectangle` struct that represents a portion of a texture based on
 * the provided parameters. The `Rectangle` struct contains the top-left
 * x-coordinate, top-left y-coordinate, width, and height of the specified
 * portion of the texture.
 */
static Rectangle getUVRectangle(const Texture2D* texture, int index, int rows, int cols) {
    float uCoord = static_cast<float>(index % cols) / static_cast<float>(cols);
    uCoord *= texture->width;

    float vCoord = static_cast<float>(index / cols) / static_cast<float>(rows);
    vCoord *= texture->height;

    float sliceWidth = texture->width / static_cast<float>(cols);
    float sliceHeight = texture->height / static_cast<float>(rows);

    return {
        uCoord, // top-left x-coord
        vCoord, // top-left y-coord
        sliceWidth, // width of slice
        sliceHeight // height of slice
    };
}

std::map<std::string, std::unique_ptr<TextureAsset>>& TextureAsset::registry() {
    static std::map<std::string, std::unique_ptr<TextureAsset>> registry;
    return registry;
}

TextureAsset* TextureAsset::get(const std::string& alias) {
    return TextureAsset::registry().at(alias).get();
}

TextureAsset::TextureAsset(const std::string& alias, const std::string& pathToTexture) :
    mTexture(LoadTexture(pathToTexture.c_str())),
    mAlias(alias) {}

TextureAsset::~TextureAsset() {
    UnloadTexture(mTexture);
}

void TextureAsset::useDefaultAnimation() {
    int32_t totalFrames = static_cast<int32_t>(mSpriteSheetDimensions.x * mSpriteSheetDimensions.y);

    for (int32_t i = 0; i < totalFrames; ++i) {
        mAnimationSpriteSheetFrames[0].push_back(i);
    }
}

Rectangle TextureAsset::getTextureArea(int32_t sliceIndex) const {
    return getUVRectangle(
        &this->getTexture2D(),
        sliceIndex,
        mSpriteSheetDimensions.x,
        mSpriteSheetDimensions.y
    );
}

TextureInstance::TextureInstance(const TextureAsset* asset) {
    this->loadAsset(asset);
}

void TextureInstance::loadAsset(const TextureAsset* asset) {
    mAsset = asset;

    mUvSize = {
        static_cast<float>(asset->getTexture2D().width),
        static_cast<float>(asset->getTexture2D().height)
    };

    // Unnecessary as this gets computed by TextureAsset::getTextureArea anyway
    if (mAsset->mType == TextureType::ATLAS) {
        mUvSize.x /= mAsset->mSpriteSheetDimensions.x;
        mUvSize.y /= mAsset->mSpriteSheetDimensions.y;
    }

    mAnimationTime = 0.0f;
    mCurrentAnimationFrameIndex = 0;
}

void TextureInstance::setAnimation(int32_t animation) {
    if (animation == mCurrentAnimation) {
        return;
    }

    mCurrentAnimation = animation;
    mCurrentAnimationFrameIndex = 0;
    mAnimationTime = 0.0f;
}

void TextureInstance::render(Rectangle destination, Vector2 origin, float angle) const {
    Rectangle textureArea;

    switch (mAsset->mType) {
        case (TextureType::SINGLE): {
            textureArea = {mUvOrigin.x, mUvOrigin.y, mUvSize.x, mUvSize.y};
            break;
        }

        case (TextureType::ATLAS): {
            const auto& frames = this->getCurrentAnimationFrames();
            if (frames.empty()) {
                return;
            }

            size_t frame = static_cast<size_t>(mCurrentAnimationFrameIndex) % frames.size();
            textureArea = mAsset->getTextureArea(frames[frame]);
            break;
        }

        default:
            break;
    }

    // Mirror texture along the y-axis for left/right-facing textures
    if (mIsFlippedX) {
        textureArea.width = -textureArea.width;
    }

    DrawTexturePro(mAsset->getTexture2D(), textureArea, destination, origin, angle, mTint);
}

void TextureInstance::animate(float deltaTime) {
    if (mAsset->mType != TextureType::ATLAS) {
        return;
    }

    const auto& frames = this->getCurrentAnimationFrames();
    if (frames.empty()) {
        return;
    }

    mAnimationTime += deltaTime;
    float secondsPerFrame = 1.0f / mFrameSpeed;

    if (mAnimationTime >= secondsPerFrame) {
        mAnimationTime = 0.0f;

        mCurrentAnimationFrameIndex++;
        mCurrentAnimationFrameIndex %= this->getCurrentAnimationFrames().size();
    }
}