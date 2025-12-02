#ifndef PLAYERHUD_HPP
#define PLAYERHUD_HPP

#include "UIScreen.hpp"

class PlayerHud : public UIScreen {
public:
  PlayerHud(class Game *game);
  void Update(float deltaTime) override;

private:
  HUDElement *mHPrect;
  HUDElement *mENrect;

  int lastHealth;
  int lastEnergy;

  float mLeftBarCenter;

  Vector3 mBarSize;
  Vector3 mBorderSize;
};

#endif