//
// Created by rapha on 5/12/2025.
// OpeningScreen.hpp

#ifndef MELLODICA_OPENINGSCREEN_H
#define MELLODICA_OPENINGSCREEN_H

#include "./UIScreen.hpp"

class OpeningScreen : public UIScreen {
public:
  OpeningScreen(class Game *game, const std::string &fontName);
  ~OpeningScreen();

  void HandleKeyPress(int key) override;
  void Update(float deltaTime) override;

private:
  Actor *mStory;
  float mStoryPosition;
  float mTimer;
};

#endif // MELLODICA_OPENINGSCREEN_H