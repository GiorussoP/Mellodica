#ifndef HUMAN_HPP
#define HUMAN_HPP

#include "actors/Combatant.hpp"
class Human : public Combatant {
public:
  Human(class Game *game, int channel, int health = 3000);
  ~Human();

protected:
  void InitializeSprite();
};

#endif