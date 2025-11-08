#ifndef TEST_SCENE_HPP
#define TEST_SCENE_HPP

#include "Scene.hpp"

class TestSceneA : public Scene {

public:
  TestSceneA(Game *game) : Scene(game) {};
  void Initialize() override;
};

class TestSceneB : public Scene {
public:
  TestSceneB(Game *game) : Scene(game) {};
  void Initialize() override;
};

#endif