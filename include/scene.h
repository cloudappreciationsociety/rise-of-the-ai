#ifndef SCENE_H
#define SCENE_H

#include "map.h"
#include "player.h"

enum class SceneId { NO_SCENE = -1, START, LEVEL_A, LEVEL_B, LEVEL_C, WIN, LOSE };

struct GameState {
    Player* player;
    std::vector<Actor*> enemies;
    std::vector<PhysicsEntity*> objects;
    Entity* bg;

    Map* map;

    SceneId nextSceneId = SceneId::NO_SCENE;
};

class Scene {
protected:
    GameState mGameState;
    Vector2 mOrigin;
    const char* mBgColorHexCode = "#000000";

public:
    Scene();
    Scene(Vector2 origin, const char* bgHexCode);

    virtual ~Scene() = default;

    virtual void initialize();
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;

    virtual void renderUI() {
        return;
    }

    virtual void shutdown();

    std::vector<PhysicsEntity*> getPlayerCollisionCandidates() const;

    GameState& getState() {
        return mGameState;
    }

    Vector2 getOrigin() const {
        return mOrigin;
    }

    const char* getBgColorHexCode() const {
        return mBgColorHexCode;
    }

    Vector2 getCameraTarget() const {
        return mGameState.player ? mGameState.player->mPosition : mOrigin;
    }
};

#endif // SCENE_H