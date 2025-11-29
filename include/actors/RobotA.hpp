#ifndef ROBOTA_HPP
#define ROBOTA_HPP

#include "actors/Combatant.hpp"

class RobotA : public Combatant {
public:
  RobotA(class Game *game, int channel, int health = 1000);
  ~RobotA();

protected:
  void InitializeSprite();
};

#endif