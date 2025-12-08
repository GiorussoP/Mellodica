#ifndef GAME_OVER_HPP
#define GAME_OVER_HPP

#include "Scene.hpp"

class GameOver : public Scene {

public:
  GameOver(Game *game) : Scene(game, scene6) {};
  void Initialize() override;
};

#endif