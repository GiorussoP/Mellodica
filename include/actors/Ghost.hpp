#ifndef GHOST_HPP
#define GHOST_HPP

#include "actors/Combatant.hpp"

class Ghost : public Combatant {
public:
  Ghost(class Game *game, int channel, int health = 1000);
  ~Ghost();

protected:
  void InitializeSprite();
};

#endif