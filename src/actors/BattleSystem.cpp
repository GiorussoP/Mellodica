#include "actors/BattleSystem.hpp"
#include "MIDIPlayer.hpp"

#include "Game.hpp"
#include "Mesh.hpp"
#include "Renderer.hpp"
#include "actors/EnemyGroup.hpp"
#include "actors/NotePlayerActor.hpp"
#include "actors/Player.hpp"

BattleSystem::BattleSystem(Game *game)
    : Actor(game), mGame(game), mInBattle(false), mEnemyNotePlayer(nullptr),
      mPlayerNotePlayer(nullptr), mCurrentEnemyGroup(nullptr) {
  mPlayerNotePlayer = new NotePlayerActor(game, false);
  mEnemyNotePlayer = new NotePlayerActor(game, true);

  mGame->AddAlwaysActive(this);

  Mesh *mesh = game->GetRenderer()->LoadMesh("plane");
  mField = new MeshComponent(this, *mesh);
  mField->SetColor(Color::Black);
  mField->SetScale(Vector3(0.0f, 0.0f, 0.0f));
  mField->SetOffset(Vector3(0.0f, -0.9f, 0.0f));

  mEdge = new MeshComponent(this, *mesh);
  mEdge->SetColor(Color::White);
  mEdge->SetScale(Vector3(0.0f, 0.0f, 0.0f));
  mEdge->SetOffset(Vector3(0.0f, -0.95f, 0.0f));
  mEdge->SetBloomed(true);

  for (int i = 0; i < 8; ++i) {
    MIDIPlayer::muteChannel(i);
    MIDIPlayer::setChannelVolume(i, 127);
  }
  for (int i = 8; i < 16; ++i) {

    i == 9 ? MIDIPlayer::muteChannel(i) : MIDIPlayer::unmuteChannel(i);
    MIDIPlayer::setChannelVolume(i, (i == 12 || i == 8 || i == 9) ? 127 : 120);
  }

  MIDIPlayer::setSpeed(0.9f);
}

BattleSystem::~BattleSystem() {}

void BattleSystem::StartBattle(EnemyGroup *enemyGroup) {
  mInBattle = true;
  mIsTransitioning = true;

  mCurrentEnemyGroup = enemyGroup;

  mBattleDir = Vector3::Normalize(mCurrentEnemyGroup->GetPosition() -
                                  mGame->GetPlayer()->GetPosition());

  Vector3 enemyPos = mCurrentEnemyGroup->GetPosition();
  Vector3 playerPos = mGame->GetPlayer()->GetPosition();
  Vector3 toPlayer = playerPos - enemyPos;
  Vector3 dist = Vector3::Dot(toPlayer, mBattleDir) * mBattleDir;

  mGame->GetCamera()->SetMode(CameraMode::Following);

  mGame->GetCamera()->SetTargetPosition(enemyPos + 0.5f * dist);

  mGame->GetCamera()->SetTargetRotation(
      Math::LookRotation(Vector3::NegUnitY, mBattleDir));

  // Position note players
  mPlayerNotePlayer->SetPosition(enemyPos + dist + mBattleDir);
  mPlayerNotePlayer->SetRotation(Math::LookRotation(mBattleDir));
  mEnemyNotePlayer->SetPosition(mCurrentEnemyGroup->GetPosition() -
                                1.5f * mBattleDir);
  mEnemyNotePlayer->SetRotation(Math::LookRotation(-1.0f * mBattleDir));

  // Mute channels 0-7
  for (int channel = 0; channel <= 7; ++channel) {
    MIDIPlayer::muteChannel(channel);
  }

  // Mute Player
  MIDIPlayer::muteChannel(12);

  // End Player Notes
  for (int i = 0; i < 128; ++i) {
    mPlayerNotePlayer->EndNote(i);
  }

  MIDIPlayer::unmuteChannel(9); // START BATTLE DRUMS
  MIDIPlayer::setSpeed(1.0f);

  mEdge->SetVisible(true);
}

void BattleSystem::EndBattle() {
  mInBattle = false;
  mIsTransitioning = true;

  std::cout << "Battle ended." << std::endl;
  // Find closest isometric direction to current camera up vector
  Vector3 camUp = mGame->GetCamera()->GetCameraUp();
  int closestDir = 0;
  float bestDot = -1.0f;
  for (int i = 0; i < 8; ++i) {
    Vector3 isoUp =
        Vector3::Transform(Vector3::UnitY, Camera::ISOMETRIC_DIRECTIONS[i]);
    float dot = Vector3::Dot(camUp, isoUp);
    if (dot > bestDot) {
      bestDot = dot;
      closestDir = i;
    }
  }
  mGame->GetCamera()->SetIsometricDirection(
      static_cast<IsometricDirections>(closestDir));
  mGame->GetCamera()->SetMode(CameraMode::Isometric);

  MIDIPlayer::muteChannel(9); // END BATTLE DRUMS
  MIDIPlayer::setSpeed(0.9f);

  // Mute channels 0-7
  for (int channel = 0; channel <= 7; ++channel) {
    MIDIPlayer::muteChannel(channel);
  }
  // Mute Player
  MIDIPlayer::muteChannel(12);

  // End all notes playing from player and enemy note players
  for (int i = 0; i < 128; ++i) {
    mPlayerNotePlayer->EndNote(i);
    mEnemyNotePlayer->EndNote(i);
  }

  mGame->GetPlayer()->SetRotation(Math::LookRotation(mBattleDir));
}

void BattleSystem::OnUpdate(float deltaTime) {
  if (mInBattle) {
    if (!mIsTransitioning) {
      Vector3 enemyPos = mCurrentEnemyGroup->GetPosition();
      Vector3 playerPos = mGame->GetPlayer()->GetPosition();
      Vector3 toPlayer = playerPos - enemyPos;
      Vector3 dist = Vector3::Dot(toPlayer, mBattleDir) * mBattleDir;

      mPlayerNotePlayer->SetPosition(enemyPos + dist + mBattleDir);

      mGame->GetCamera()->SetTargetPosition(
          enemyPos - 0.5f * mCurrentEnemyGroup->GetRadius() * mBattleDir);

      auto notes = MIDIPlayer::pollNoteEvents();
      for (auto enemy : mCurrentEnemyGroup->GetEnemies()) {
        if (enemy->GetCombatantState() != CombatantState::Dead) {
          for (auto note : notes) {

            if (enemy->GetChannel() == note.channel) {
              if (note.noteOn) {
                mEnemyNotePlayer->PlayNote(note.note, note.channel);
                enemy->SetCombatantState(CombatantState::Attacking);
                enemy->SetPosition(
                    mEnemyNotePlayer->GetNotePosition(note.note) + mBattleDir);
              } else {
                mEnemyNotePlayer->EndNote(note.note);
                enemy->SetCombatantState(CombatantState::Idle);
                /*
                // GOTO NEXT NOTE BEHAVIOUR - DISABLED FOR NOW
                if (note.hasNextNote) {
                  Vector3 nextNotePos =
                      mEnemyNotePlayer->GetNotePosition(note.nextNote);

                  float distance =
                      Vector3::Distance(enemy->GetPosition(), nextNotePos);
                  float speed = distance / Math::Max(note.nextNoteTime, 0.1);

                  enemy->SetCombatantState(CombatantState::Dodging);
                  enemy->GoToPositionAtSpeed(nextNotePos, speed);
                }

                */
              }
            }
          }

        } else {
          // Ensure all notes are ended for dead enemies
          for (int i = 0; i < MAX_NOTES; ++i) {
            auto note = mEnemyNotePlayer->GetActiveNotes()[i];
            if (note != nullptr &&
                enemy->GetChannel() == note->GetMidiChannel()) {
              mEnemyNotePlayer->EndNote(note->GetNote());
            }
          }
          MIDIPlayer::muteChannel(enemy->GetChannel());
        }
      }
      // Field resizing

      dist =
          (mPlayerNotePlayer->GetPosition() - mEnemyNotePlayer->GetPosition());
      SetPosition(mEnemyNotePlayer->GetPosition() + 0.5f * dist);

      mField->SetScale(Vector3(6.0f, 1.0f, dist.Length() + 0.5f));

      mEdge->SetScale(mField->GetScale() + Vector3(0.25f, 0.0f, 0.25f));

    } else {
      // Is transitioning into battle

      Vector3 dist =
          (mPlayerNotePlayer->GetPosition() - mEnemyNotePlayer->GetPosition());
      SetPosition(mEnemyNotePlayer->GetPosition() + 0.5f * dist);

      mField->SetScale(Vector3::Lerp(mField->GetScale(),
                                     Vector3(6.0f, 1.0f, dist.Length() + 0.5f),
                                     4.0f * deltaTime));
      mEdge->SetScale(Vector3::Lerp(
          mEdge->GetScale(), mField->GetScale() + Vector3(0.25f, 0.0f, 0.25f),
          4.0f * deltaTime));

      mRotation = Quaternion::Slerp(mRotation, Math::LookRotation(mBattleDir),
                                    4.0f * deltaTime);
      mPosition = Vector3::Lerp(mPosition,
                                mEnemyNotePlayer->GetPosition() + 0.5f * dist,
                                4.0f * deltaTime);

      if (Vector3::Dot(Vector3::NegUnitY,
                       mGame->GetCamera()->GetCameraForward()) > 0.99) {
        mIsTransitioning = false;

        auto &allies = mGame->GetPlayer()->GetActiveAllies();
        auto &enemies = mCurrentEnemyGroup->GetEnemies();

        std::cout << "Battle started with " << allies.size() << " allies and "
                  << enemies.size() << " enemies." << std::endl;

        // Unmute allies and enemies channels
        for (auto ally : allies) {
          MIDIPlayer::unmuteChannel(ally->GetChannel());
        }
        for (auto enemy : enemies) {
          MIDIPlayer::unmuteChannel(enemy->GetChannel());
        }

        mRotation = Math::LookRotation(mBattleDir);
        mPosition = mEnemyNotePlayer->GetPosition() + 0.5f * dist;
      }
    }
  } else {
    Vector3 pos = mGame->GetPlayer()->GetPosition() +
                  Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f),
                                     mGame->GetPlayer()->GetRotation());
    if (!mIsTransitioning) {
      // not battling

      mPlayerNotePlayer->SetPosition(pos);
      mPlayerNotePlayer->SetRotation(mGame->GetPlayer()->GetRotation());
      mPosition = pos;
      mRotation = mGame->GetPlayer()->GetRotation();

      bool visible = false;
      for (auto note : mPlayerNotePlayer->GetActiveNotes()) {

        if (note != nullptr) {
          visible = true;
          break;
        }
      }
      mEdge->SetVisible(visible);

    } else {
      // Is transitioning to outside battle
      mField->SetScale(Vector3::Lerp(
          mField->GetScale(), Vector3(6.0f, 1.0f, 0.0f), 4.0f * deltaTime));
      mEdge->SetScale(Vector3::Lerp(
          mEdge->GetScale(), Vector3(6.0f, 1.0f, 0.25f), 4.0f * deltaTime));

      mPosition = Vector3::Lerp(mPosition, pos, 4.0f * deltaTime);
      mRotation = Quaternion::Slerp(
          mRotation, mGame->GetPlayer()->GetRotation(), 4.0f * deltaTime);
      if (mField->GetScale().z < 0.26f) {
        mIsTransitioning = false;
        mField->SetScale(Vector3(6.0f, 1.0f, 0.0f));
        mEdge->SetScale(Vector3(6.0f, 1.0f, 0.25f));

        std::cout << "Exited battle mode." << std::endl;
      }
    }
  }
}