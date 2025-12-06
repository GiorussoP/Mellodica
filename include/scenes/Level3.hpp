#ifndef LEVEL3_HPP
#define LEVEL3_HPP

#include "scenes/Scene.hpp"

class Level3 : public Scene {
public:
  Level3(class Game *game) : Scene(game) {}
  void Initialize() override;
  void LoadLevel(const std::string &levelPath) override;
};

#endif