#include "EnemyGroup.hpp"

#include "Game.hpp"

EnemyGroup::EnemyGroup(class Game *game, std::vector<Combatant *> enemies,
                       float radius)
    : Actor(game), mRadius(radius) {
  mGame->AddAlwaysActive(this);
  const float minRadius = 1.0f;
  for (unsigned int i = 0; i < enemies.size(); ++i) {
    Combatant *combatant = enemies[i];
    mEnemies.push_back(combatant);
    float angle = (static_cast<float>(i) / enemies.size()) * Math::TwoPi;
    Vector3 offset = Vector3(minRadius * Math::Cos(angle), 0.0f,
                             minRadius * Math::Sin(angle));
    combatant->SetPosition(mPosition + offset);
  }
}

EnemyGroup::~EnemyGroup() {}

void EnemyGroup::OnUpdate(float deltaTime) {

  bool everyoneDead = true;
  for (auto enemy : mEnemies) {
    if (enemy->GetCombatantState() != CombatantState::Dead) {
      everyoneDead = false;
      break;
    }
  }

  if (!mGame->GetBattleSystem()->IsInBattle()) {
    if ((mGame->GetPlayer()->GetPosition() - mPosition).LengthSq() <
            mRadius * mRadius &&
        !everyoneDead) {
      mGame->GetBattleSystem()->StartBattle(this);
    }
  } else {
    if (mGame->GetBattleSystem()->GetCurrentEnemyGroup() == this) {

      Vector3 playerDist = (mGame->GetPlayer()->GetPosition() - mPosition);
      Vector3 battleDir = mGame->GetBattleSystem()->GetBattleDirection();
      Vector3 flatDist = (Vector3::Dot(playerDist, battleDir) * battleDir);
      Vector3 lateral = playerDist - flatDist;

      bool correctClose = flatDist.LengthSq() < 1.5f * 1.5f;
      bool correctFar = lateral.LengthSq() > 3.0f * 3.0f;

      Vector3 desired_flatDist = correctClose ? battleDir * (-1.5f) : flatDist;
      Vector3 desired_lateral = lateral;
      if (correctFar) {
        Vector3 lateralDir = lateral;
        lateralDir.Normalize();
        desired_lateral = lateralDir * 3.0f;
      }

      Vector3 newPos = mPosition + desired_flatDist + desired_lateral;
      mGame->GetPlayer()->SetPosition(newPos);

      // Anullate movement in corrected directions
      Vector3 velocity =
          mGame->GetPlayer()->GetComponent<RigidBodyComponent>()->GetVelocity();
      if (correctClose) {
        velocity -= Vector3::Dot(velocity, battleDir) * battleDir;
      }
      if (correctFar) {
        Vector3 lateralDir = lateral;
        lateralDir.Normalize();
        velocity -= Vector3::Dot(velocity, lateralDir) * lateralDir;
      }

      if (velocity.LengthSq() < 0.1f * 0.1f) {
        velocity = Vector3::Zero;
      }
      mGame->GetPlayer()->GetComponent<RigidBodyComponent>()->SetVelocity(
          velocity);

      if (everyoneDead || flatDist.LengthSq() > mRadius * mRadius + 1.5f) {
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