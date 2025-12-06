#ifndef BATTLESYSTEM_HPP
#define BATTLESYSTEM_HPP

#include "Actor.hpp"

class BattleSystem : public Actor {
public:
  BattleSystem(class Game *game);
  ~BattleSystem();

  void StartBattle(class EnemyGroup *enemyGroup);
  void EndBattle(bool won = false);

  bool IsInBattle() { return mInBattle; }
  bool IsTransitioning() { return mIsTransitioning; }

  // Battle direction getter
  Vector3 GetBattleDirection() const { return mBattleDir; }

  void OnUpdate(float deltaTime) override;

  EnemyGroup *GetCurrentEnemyGroup() { return mCurrentEnemyGroup; }

  class NotePlayerActor *GetPlayerNotePlayer() { return mPlayerNotePlayer; }
  class NotePlayerActor *GetEnemyNotePlayer() { return mEnemyNotePlayer; }

  class MeshComponent *GetFieldMesh() { return mField; }
  class MeshComponent *GetEdgeMesh() { return mEdge; }

  unsigned int GetPlayerPositionNote();

private:
  class Game *mGame;
  class NotePlayerActor *mPlayerNotePlayer;
  class NotePlayerActor *mEnemyNotePlayer;

  class EnemyGroup *mCurrentEnemyGroup;

  bool mInBattle;
  bool mIsTransitioning;

  Vector3 mBattleDir;

  class MeshComponent *mField;
  class MeshComponent *mEdge;

  class BattleScreen *mBattleScreen;
};

#endif