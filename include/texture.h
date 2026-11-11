#ifndef TEXTURE_H
#define TEXTURE_H

#include <memory>

#include "lib.h"

enum class TextureType { SINGLE, ATLAS };

// Entites do not own but are rather assigned textures.
// TextureAsset is responsible for loading and unloading raylib textures.
class TextureAsset {
private:
    Texture2D mTexture; // Immutable once loaded
    const std::string mAlias;

public:
    TextureType mType = TextureType::SINGLE;
    std::map<int32_t, std::vector<int32_t>> mAnimationSpriteSheetFrames;
    Vector2 mSpriteSheetDimensions;

public:
    // Similar class-centralized registry spiel to Entity
    static std::map<std::string, std::unique_ptr<TextureAsset>>& registry();
    static TextureAsset* get(const std::string& alias);

    template<typename T>
    static T* create(T* object) {
        auto ptr = std::unique_ptr<T>(object);
        T* outPtr = ptr.get();
        TextureAsset::registry().emplace(ptr->mAlias, std::move(ptr));
        return outPtr;
    }

    TextureAsset(const std::string& alias, const std::string& pathToTexture);
    ~TextureAsset();

    void useDefaultAnimation();
    Rectangle getTextureArea(int32_t sliceIndex) const;

    const Texture2D& getTexture2D() const {
        return mTexture;
    }

    int32_t getWidth() const {
        return mTexture.width;
    }

    int32_t getHeight() const {
        return mTexture.height;
    }
};

class TextureInstance {
private:
    static constexpr uint32_t DEFAULT_FRAME_SPEED = 14;

private:
    float mAnimationTime = 0.0f;
    int32_t mCurrentAnimationFrameIndex = 0;

public:
    const TextureAsset* mAsset = nullptr;

    Vector2 mUvOrigin = {0.0f, 0.0f};
    Vector2 mUvSize;
    Color mTint = WHITE;

    // For animations
    int32_t mCurrentAnimation = 0;

    uint32_t mFrameSpeed = TextureInstance::DEFAULT_FRAME_SPEED;

    bool mIsFlippedX = false;
    bool mShouldAnimationLoop = true;
    bool mIsAnimationCompleted = false; // Always false if animation is looping

private:
    const std::vector<int32_t>& getCurrentAnimationFrames() const {
        return mAsset->mAnimationSpriteSheetFrames.at(mCurrentAnimation);
    }

public:
    TextureInstance() = default;
    TextureInstance(const TextureAsset* asset);

    void loadAsset(const TextureAsset* asset);
    void setAnimation(int32_t animation);
    void render(Rectangle destination, Vector2 origin, float angle) const;
    void animate(float deltaTime);

    void setAnimationDuration(float duration) {
        mFrameSpeed = static_cast<uint32_t>(this->getCurrentAnimationFrames().size() / duration);
    }
};

#endif