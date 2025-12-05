#ifndef BATTLESCREEN_HPP
#define BATTLESCREEN_HPP

#include "UIScreen.hpp"

class BattleScreen : public UIScreen {

public:
  BattleScreen(class Game *game, const std::string &fontName);
  ~BattleScreen();

  void Update(float deltaTime);

private:
  std::vector<HUDElement *> mEnemyHPrects;
  std::vector<int> mEnemyLastHealths;
  std::vector<TextElement *> mEnemyHPTexts;

  std::vector<HUDElement *> mAllyHPrects;
  std::vector<int> mAllyLastHealths;
  std::vector<TextElement *> mAllyHPTexts;

  float mLeftBarCenter;
  float mRightBarCenter;

  Vector3 mBarSize;
  Vector3 mBorderSize;
};

#endif