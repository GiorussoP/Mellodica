#ifndef LEVEL2_HPP
#define LEVEL2_HPP

#include "scenes/Scene.hpp"

class Level2 : public Scene {
public:
  Level2(class Game *game) : Scene(game, scene2) {}
  void Initialize() override;
  void LoadLevel(const std::string &levelPath) override;
};

#endif