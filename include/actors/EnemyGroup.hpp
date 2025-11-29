#ifndef ENEMYGROUP_HPP
#define ENEMYGROUP_HPP

#include "Actor.hpp"
#include "actors/Combatant.hpp"
#include "components/ColliderComponent.hpp"
#include <vector>

struct EnemyInfo {
  unsigned int channel;
  int health;
};

class EnemyGroup : public Actor {

public:
  EnemyGroup(class Game *game, std::vector<Combatant *> enemies,
             float radius = 8.0f);
  ~EnemyGroup();

  void OnUpdate(float deltaTime) override;
  void SetPosition(const Vector3 pos) override;

  float GetRadius() const { return mRadius; }

  std::vector<Combatant *> &GetEnemies() { return mEnemies; }

private:
  std::vector<Combatant *> mEnemies;
  float mRadius;
};

#endif
