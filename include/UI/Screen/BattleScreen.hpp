#ifndef BATTLESCREEN_HPP
#define BATTLESCREEN_HPP

#include "UIScreen.hpp"

class BattleScreen : public UIScreen {

public:
  BattleScreen(class Game *game, const std::string &fontName);
  ~BattleScreen();

  void Update(float deltaTime);

private:
  HUDElement *mHPrect;
  HUDElement *mENrect;

  std::vector<HUDElement *> mEnemyHPrects;

  std::vector<HUDElement *> mAllyHPrects;

  float mLeftBarCenter;
  float mRightBarCenter;

  Vector3 mBarSize;
  Vector3 mBorderSize;
};

#endif