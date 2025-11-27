#include "EnemyGroup.hpp"

#include "Game.hpp"

EnemyGroup::EnemyGroup(class Game *game, std::vector<EnemyInfo> enemies,
                       float radius)
    : Actor(game), mRadius(radius) {

  mGame->AddAlwaysActive(this);

  // Create Combatant for each enemy channel
  const float minRadius = 1.0f;
  for (unsigned int i = 0; i < enemies.size(); ++i) {
    Combatant *combatant =
        new Combatant(game, enemies[i].channel, enemies[i].health);
    mEnemies.push_back(combatant);

    // Set position around circle
    float angle = (static_cast<float>(i) / enemies.size()) * Math::TwoPi;
    Vector3 offset = Vector3(minRadius * Math::Cos(angle), 0.0f,
                             minRadius * Math::Sin(angle));
    combatant->SetPosition(mPosition + offset);
  }
}

EnemyGroup::~EnemyGroup() {}

void EnemyGroup::OnUpdate(float deltaTime) {
  // Update positions of enemies in a circle around the group position

  if (!mGame->GetBattleSystem()->IsInBattle()) {
    if ((mGame->GetPlayer()->GetPosition() - mPosition).LengthSq() <
        mRadius * mRadius) {
      mGame->GetBattleSystem()->StartBattle(this);
    }
  } else {
    if (mGame->GetBattleSystem()->GetCurrentEnemyGroup() == this) {

      Vector3 playerDist = (mGame->GetPlayer()->GetPosition() - mPosition);
      Vector3 battleDir = mGame->GetBattleSystem()->GetBattleDirection();
      Vector3 flatDist = (Vector3::Dot(playerDist, battleDir) * battleDir);

      if (flatDist.LengthSq() > mRadius * mRadius + 1.0f ||
          (playerDist - flatDist).LengthSq() > 3.4f * 3.4f) {
        mGame->GetBattleSystem()->EndBattle();
      }
    }
  }
}

void EnemyGroup::SetPosition(const Vector3 pos) {
  Actor::SetPosition(pos);

  // Update positions of enemies in a circle around the group position
  const float minRadius = 1.0f;
  for (size_t i = 0; i < mEnemies.size(); ++i) {
    float angle = (static_cast<float>(i) / mEnemies.size()) * Math::TwoPi;
    Vector3 offset = Vector3(minRadius * Math::Cos(angle), 0.0f,
                             minRadius * Math::Sin(angle));
    mEnemies[i]->SetPosition(mPosition + offset);
  }
}