//
// Created by rapha on 5/12/2025.
// GameOverScreen.hpp

#ifndef MELLODICA_GAMEOVERSCREEN_H
#define MELLODICA_GAMEOVERSCREEN_H

#include "./UIScreen.hpp"

class GameOverScreen : public UIScreen {
public:
  GameOverScreen(class Game *game, const std::string &fontName);
  ~GameOverScreen();

  void HandleKeyPress(int key) override;
  void Update(float deltaTime) override {};
};

#endif // MELLODICA_GAMEOVERSCREEN_H