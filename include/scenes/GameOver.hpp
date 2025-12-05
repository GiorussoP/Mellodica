#ifndef GAME_OVER_HPP
#define GAME_OVER_HPP

#include "Scene.hpp"

class GameOver : public Scene {

public:
  GameOver(Game *game) : Scene(game) {};
  void Initialize() override;
};

#endif