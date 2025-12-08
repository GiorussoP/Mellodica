#ifndef LEVEL3_HPP
#define LEVEL3_HPP

#include "scenes/Scene.hpp"

class Level3 : public Scene {
public:
  Level3(class Game *game) : Scene(game, scene3) {}
  void Initialize() override;
  void LoadLevel(const std::string &levelPath) override;
};

#endif