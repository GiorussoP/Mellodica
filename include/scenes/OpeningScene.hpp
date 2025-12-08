#ifndef OPENING_SCENE_HPP
#define OPENING_SCENE_HPP

#include "Scene.hpp"

class OpeningScene : public Scene {

public:
  OpeningScene(Game *game) : Scene(game, scene4) {};
  void Initialize() override;
};

#endif