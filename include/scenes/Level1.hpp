#ifndef LEVEL1_HPP
#define LEVEL1_HPP

#include "scenes/Scene.hpp"

class Level1 : public Scene {
public:
  Level1(class Game *game) : Scene(game) {}
  void Initialize() override;
};

#endif