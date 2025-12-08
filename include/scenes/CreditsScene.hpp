#ifndef CREDITS_SCENE_HPP
#define CREDITS_SCENE_HPP

#include "Scene.hpp"

class CreditsScene : public Scene {

public:
  CreditsScene(Game *game) : Scene(game, scene7) {};
  void Initialize() override;
};

#endif