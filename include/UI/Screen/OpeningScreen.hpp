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
  HUDElement *mStory0;
  HUDElement *mStory1;
  HUDElement *mStory2;
  HUDElement *mStory3;
  HUDElement *mStory4;
  HUDElement *mStory5;
  HUDElement *mStory6;
  TextElement *mStoryText;

  unsigned int mImageIndex;
  float mStoryPosition;
  float mTimer;
};

#endif // MELLODICA_OPENINGSCREEN_H