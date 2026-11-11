#ifndef MENUS_H
#define MENUS_H

#include "scene.h"

class StartMenu: public Scene {
public:
    using Scene::Scene;

    void initialize() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

class WinMenu: public Scene {
public:
    using Scene::Scene;

    void initialize() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

class LoseMenu: public Scene {
public:
    using Scene::Scene;

    void initialize() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};

#endif