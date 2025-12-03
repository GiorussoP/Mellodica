#ifndef SHINE_ACTOR_HPP
#define SHINE_ACTOR_HPP

#include "Actor.hpp"

class ShineActor : public Actor {
public:
  ShineActor(class Game *game, Vector3 color, bool autoDestroy = true);
  void OnUpdate(float deltaTime) override;

  void Start(float lifetime);
  void SetAutoDestroy(bool autoDestroy) { mAutoDestroy = autoDestroy; }

private:
  class SpriteComponent *mSpriteComponent;
  float mLifetime;
  bool mAutoDestroy;
};
#endif