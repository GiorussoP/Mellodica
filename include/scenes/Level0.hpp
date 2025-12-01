#ifndef LEVEL0_HPP
#define LEVEL0_HPP

#include "scenes/Scene.hpp"

class Level0 : public Scene {
public:
  Level0(class Game *game) : Scene(game) {}
  void Initialize() override;
};

#endif