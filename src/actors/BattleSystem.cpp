#include "actors/BattleSystem.hpp"
#include "AssetLoader.hpp"
#include "MIDI/MIDIPlayer.hpp"

#include "Game.hpp"
#include "UI/Screen/BattleScreen.hpp"
#include "actors/EnemyGroup.hpp"
#include "actors/NotePlayerActor.hpp"
#include "actors/Player.hpp"
#include "render/Mesh.hpp"
#include "render/Renderer.hpp"

BattleSystem::BattleSystem(Game *game)
    : Actor(game), mGame(game), mInBattle(false), mIsTransitioning(false),
      mEnemyNotePlayer(nullptr), mPlayerNotePlayer(nullptr),
      mCurrentEnemyGroup(nullptr), mBattleScreen(nullptr) {
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
  mEdge->SetScale(Vector3(1.0f, 1.0f, 0.25f));
  mEdge->SetOffset(Vector3(0.0f, -0.95f, 0.0f));
  mEdge->SetBloomed(true);

  for (int i = 0; i < 8; ++i) {
    MIDIPlayer::muteChannel(i);
    MIDIPlayer::setChannelVolume(i, 127);
  }
  for (int i = 8; i < 16; ++i) {

    i == 9 ? MIDIPlayer::muteChannel(i) : MIDIPlayer::unmuteChannel(i);
    MIDIPlayer::setChannelVolume(i, (i == 12) ? 127 : 100);
  }

  MIDIPlayer::setSpeed(0.7f);
  SynthEngine::setPan(12, 64);
}

BattleSystem::~BattleSystem() {
  if (mGame->GetBattleSystem() == this) {
    mGame->SetBattleSystem(nullptr);
  }
}

void BattleSystem::StartBattle(EnemyGroup *enemyGroup) {
  mGame->SaveState();

  mInBattle = true;
  mIsTransitioning = true;

  mCurrentEnemyGroup = enemyGroup;

  mBattleDir = Vector3::Normalize(mCurrentEnemyGroup->GetPosition() -
                                  mGame->GetPlayer()->GetPosition());

  if (mBattleDir.LengthSq() < 0.001f) {
    mBattleDir = Vector3::UnitX;
  }

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
                                0.5f * mBattleDir);
  mEnemyNotePlayer->SetRotation(Math::LookRotation(-1.0f * mBattleDir));

  // Mute channels 0-7
  for (int channel = 0; channel <= 7; ++channel) {
    MIDIPlayer::muteChannel(channel);
  }

  // Mute Player
  MIDIPlayer::muteChannel(12);

  // End Player Notes
  for (int i = 0; i < 12; ++i) {
    mPlayerNotePlayer->EndNote(i);
  }

  MIDIPlayer::unmuteChannel(9); // START BATTLE DRUMS
  MIDIPlayer::setSpeed(1.0f);

  mEdge->SetVisible(true);
  mEdge->SetScale(Vector3(1.0f, 1.0f, 0.25f));
  mField->SetVisible(true);
  mField->SetScale(Vector3(1.0f, 1.0f, 0.0f));
  mPosition = mGame->GetPlayer()->GetPosition() + mBattleDir;
  mRotation = Math::LookRotation(mBattleDir);

  // Setting enemy positions:
  Vector3 right = Vector3::Cross(Vector3::UnitY, mBattleDir);
  for (int i = 0; i < mCurrentEnemyGroup->GetEnemies().size(); ++i) {
    Vector3 pos =
        mEnemyNotePlayer->GetPosition() + right * i * 1.5f -
        right * ((mCurrentEnemyGroup->GetEnemies().size() - 1) * 1.5f / 2.0f);

    pos += 1.0f * mBattleDir;
    mCurrentEnemyGroup->GetEnemies()[i]->GoToPositionAtSpeed(pos, 1.5f);
    mCurrentEnemyGroup->GetEnemies()[i]->SetCombatantState(
        CombatantState::Moving);

    mCurrentEnemyGroup->GetEnemies()[i]->SetRotation(
        Math::LookRotation(-1.0f * mBattleDir));

    // Restore enemy health at start of battle
    mCurrentEnemyGroup->GetEnemies()[i]->SetHealth(
        mCurrentEnemyGroup->GetEnemies()[i]->GetMaxHealth());
  }

  // Setting ally positions
  Vector3 allyRight = Vector3::Cross(Vector3::NegUnitY, mBattleDir);

  for (int i = 0; i < mGame->GetPlayer()->GetActiveAllies().size(); ++i) {
    if (mGame->GetPlayer()->GetActiveAllies()[i]->GetCombatantState() ==
        CombatantState::Dead) {
      continue;
    }
    Vector3 pos =
        mGame->GetPlayer()->GetPosition() + allyRight * i * 1.5f -
        allyRight *
            ((mGame->GetPlayer()->GetActiveAllies().size() - 1) * 1.5f / 2.0f);
    pos = pos - 1.0f * mBattleDir;
    mGame->GetPlayer()->GetActiveAllies()[i]->GoToPositionAtSpeed(pos, 1.5f);
    mGame->GetPlayer()->GetActiveAllies()[i]->SetCombatantState(
        CombatantState::Moving);
  }

  // Battle start sound effect - tense rising chords
  MIDIPlayer::playSequence({{0.0f, 15, 60, true, 127},

                            {0.1f, 15, 63, true, 127},

                            {0.1f, 15, 67, true, 127},
                            {0.7f, 15, 67, false},
                            {0.0f, 15, 60, false},
                            {0.0f, 15, 63, false}});
  // Screen
  mBattleScreen =
      new BattleScreen(mGame, getAssetPath("fonts/MedodicaRegular.otf"));
}

void BattleSystem::EndBattle(bool won) {
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
  MIDIPlayer::setSpeed(0.7f);

  // Mute enemies
  for (auto enemy : mCurrentEnemyGroup->GetEnemies()) {
    MIDIPlayer::muteChannel(enemy->GetChannel());
    if (enemy->GetCombatantState() != CombatantState::Dead)
      enemy->SetCombatantState(CombatantState::Idle);
  }

  // Mute allies
  for (auto ally : mGame->GetPlayer()->GetActiveAllies()) {
    MIDIPlayer::muteChannel(ally->GetChannel());
    if (ally->GetCombatantState() != CombatantState::Dead)
      ally->SetCombatantState(CombatantState::Idle);
  }

  // Mute Player
  MIDIPlayer::muteChannel(12);

  // End all notes playing from player and enemy note players
  for (int i = 0; i < 12; ++i) {
    mPlayerNotePlayer->EndNote(i);
    mEnemyNotePlayer->EndNote(i);
  }

  // Destroy dead allies

  for (auto it = mGame->GetPlayer()->GetActiveAllies().begin();
       it != mGame->GetPlayer()->GetActiveAllies().end();) {
    if ((*it)->GetCombatantState() == CombatantState::Dead) {
      (*it)->SetState(ActorState::Destroy);
      it = mGame->GetPlayer()->GetActiveAllies().erase(it);
    } else {
      (*it)->SetCombatantState(CombatantState::Idle);
      ++it;
    }
  }

  // Transfer new dead enemies to player allies if won
  for (auto it = mCurrentEnemyGroup->GetEnemies().begin();
       it != mCurrentEnemyGroup->GetEnemies().end();) {
    if ((*it)->GetCombatantState() == CombatantState::Dead) {
      auto deadEnemy = *it;

      bool hasChannel = false;
      for (auto ally : mGame->GetPlayer()->GetActiveAllies()) {
        if (ally->GetChannel() == deadEnemy->GetChannel()) {
          hasChannel = true;
          break;
        }
      }

      if (won && !hasChannel) {
        deadEnemy->SetCombatantState(CombatantState::Idle);
        deadEnemy->SetMaxHealth(deadEnemy->GetMaxHealth() / 2);
        deadEnemy->SetHealth(deadEnemy->GetMaxHealth());
        mGame->GetPlayer()->GetActiveAllies().push_back(deadEnemy);
      } else {
        deadEnemy->SetState(ActorState::Destroy);
      }
      it = mCurrentEnemyGroup->GetEnemies().erase(it);

    } else {
      (*it)->SetCombatantState(CombatantState::Idle);
      ++it;
    }
  }

  mGame->GetPlayer()->SetRotation(Math::LookRotation(mBattleDir));

  mBattleScreen->Close();
  mBattleScreen = nullptr;

  // Clear the enemy group pointer to avoid accessing destroyed object
  mCurrentEnemyGroup = nullptr;
}

void BattleSystem::OnUpdate(float deltaTime) {
  if (mInBattle) {
    if (!mIsTransitioning && mCurrentEnemyGroup) {
      Vector3 enemyPos = mCurrentEnemyGroup->GetPosition();
      Vector3 playerPos = mGame->GetPlayer()->GetPosition();
      Vector3 toPlayer = playerPos - enemyPos;
      Vector3 dist = Vector3::Dot(toPlayer, mBattleDir) * mBattleDir;

      mPlayerNotePlayer->SetPosition(enemyPos + dist + mBattleDir);

      mGame->GetCamera()->SetTargetPosition(
          enemyPos - 0.5f * mCurrentEnemyGroup->GetRadius() * mBattleDir);

      auto notes = MIDIPlayer::pollNoteEvents();

      // Handle enemy notes
      std::array<bool, 8> channelActive = {false};

      if (mCurrentEnemyGroup) {
        for (auto enemy : mCurrentEnemyGroup->GetEnemies()) {
          if (enemy->GetCombatantState() != CombatantState::Dead) {
            channelActive[enemy->GetChannel()] = true;
            for (auto note : notes) {

              if (enemy->GetChannel() == note.channel) {
                auto activeNote = mEnemyNotePlayer->GetActiveNote(note.note);

                if (note.noteOn) {
                  // End existing note
                  if (activeNote != nullptr)
                    mEnemyNotePlayer->EndNote(note.note);

                  // Play note
                  mEnemyNotePlayer->PlayNote(note.note, note.channel);

                  // Update enemy state and position
                  enemy->SetCombatantState(CombatantState::Attacking);
                  enemy->SetPosition(
                      mEnemyNotePlayer->GetNotePosition(note.note) +
                      mBattleDir);

                } else {
                  // Don't stop other's notes
                  if (!(activeNote == nullptr ||
                        (activeNote != nullptr &&
                         activeNote->GetMidiChannel() != note.channel))) {
                    // Stop note if playing this channel note
                    mEnemyNotePlayer->EndNote(note.note);
                  }

                  Vector3 nextNotePos =
                      mEnemyNotePlayer->GetNotePosition(note.nextNote) +
                      mBattleDir;

                  if (Vector3::Distance(enemy->GetPosition(), nextNotePos) <
                      0.5f) {
                    // Go to random position if already on the note pos
                    nextNotePos =
                        mEnemyNotePlayer->GetNotePosition(note.note + 6) +
                        mBattleDir;
                  }
                  float nextNoteTime = note.nextNoteTime;

                  float distance =
                      Vector3::Distance(enemy->GetPosition(), nextNotePos);

                  enemy->GoToPositionAtSpeed(
                      nextNotePos, distance / Math::Max(nextNoteTime, 0.1f));

                  enemy->SetCombatantState(CombatantState::Moving);
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
          }
        }
      } // End mCurrentEnemyGroup null check

      // Handle ally notes
      for (auto ally : mGame->GetPlayer()->GetActiveAllies()) {
        if (ally->GetCombatantState() != CombatantState::Dead) {
          channelActive[ally->GetChannel()] = true;
          for (auto note : notes) {

            if (ally->GetChannel() == note.channel) {
              auto activeNote = mPlayerNotePlayer->GetActiveNote(note.note);

              if (note.noteOn) {
                // End existing note
                if (activeNote != nullptr)
                  mPlayerNotePlayer->EndNote(note.note);

                // Play note
                mPlayerNotePlayer->PlayNote(note.note, note.channel);

                // Update ally state and position
                ally->SetCombatantState(CombatantState::Attacking);
                ally->SetPosition(
                    mPlayerNotePlayer->GetNotePosition(note.note) -
                    2.0f * mBattleDir);

              } else {

                // Don't stop other's notes
                if (!(activeNote == nullptr ||
                      (activeNote != nullptr &&
                       activeNote->GetMidiChannel() != note.channel))) {

                  mPlayerNotePlayer->EndNote(note.note);
                }
                Vector3 nextNotePos =
                    mPlayerNotePlayer->GetNotePosition(note.nextNote) -
                    2.0f * mBattleDir;

                if (Vector3::Distance(ally->GetPosition(), nextNotePos) <
                    0.5f) {
                  // Go to random position if already on the note pos
                  nextNotePos =
                      mPlayerNotePlayer->GetNotePosition(note.note + 6) -
                      2.0f * mBattleDir;
                }

                float nextNoteTime = note.nextNoteTime;

                float distance =
                    Vector3::Distance(ally->GetPosition(), nextNotePos);

                ally->GoToPositionAtSpeed(
                    nextNotePos, distance / Math::Max(nextNoteTime, 0.1f));

                ally->SetCombatantState(CombatantState::Moving);
              }
            }
          }

        } else {
          // Ensure all notes are ended for dead allies
          for (int i = 0; i < MAX_NOTES; ++i) {
            auto note = mPlayerNotePlayer->GetActiveNotes()[i];
            if (note != nullptr &&
                ally->GetChannel() == note->GetMidiChannel()) {
              mPlayerNotePlayer->EndNote(note->GetNote());
            }
          }
        }
      }

      // Muting channels with no alive enemies or allies
      for (int channel = 0; channel < 8; ++channel) {
        if (!channelActive[channel]) {
          MIDIPlayer::muteChannel(channel);
        }
      }

      // Field resizing

      dist =
          (mPlayerNotePlayer->GetPosition() - mEnemyNotePlayer->GetPosition());

      mPosition = mEnemyNotePlayer->GetPosition() + 0.5f * dist;

      mField->SetScale(Vector3(6.0f, 1.0f, dist.Length() + 0.5f));

      mEdge->SetScale(mField->GetScale() + Vector3(0.25f, 0.0f, 0.25f));

    } else {
      // Is transitioning into battle

      Vector3 dist =
          (mPlayerNotePlayer->GetPosition() - mEnemyNotePlayer->GetPosition());

      mField->SetScale(Vector3::Lerp(mField->GetScale(),
                                     Vector3(6.0f, 1.0f, dist.Length() + 0.5f),
                                     4.0f * deltaTime));

      mEdge->SetScale(mField->GetScale() + Vector3(0.25f, 0.0f, 0.25f));

      mRotation = Quaternion::Slerp(mRotation, Math::LookRotation(mBattleDir),
                                    4.0f * deltaTime);
      mPosition = Vector3::Lerp(mPosition,
                                mEnemyNotePlayer->GetPosition() + 0.5f * dist,
                                4.0f * deltaTime);

      if (mField->GetScale().z > dist.Length() + 0.49f &&
          Vector3::Dot(mGame->GetCamera()->GetCameraForward(),
                       Vector3::NegUnitY) > 0.99f) {
        mIsTransitioning = false;

        auto &allies = mGame->GetPlayer()->GetActiveAllies();

        if (mCurrentEnemyGroup) {
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
        }

        mRotation = Math::LookRotation(mBattleDir);
        mPosition = mEnemyNotePlayer->GetPosition() + 0.5f * dist;

        mField->SetOffset(Vector3(0.0f, -0.55f, 0.0f));
        mEdge->SetOffset(Vector3(0.0f, -0.6f, 0.0f));
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
      mField->SetVisible(visible);

    } else {

      // Is transitioning to outside battle
      mGame->GetCamera()->SetTargetPosition(mGame->GetPlayer()->GetPosition());

      mField->SetScale(Vector3::Lerp(
          mField->GetScale(), Vector3(1.0f, 1.0f, 0.0f), 4.0f * deltaTime));

      mEdge->SetScale(Vector3::Lerp(
          mEdge->GetScale(), Vector3(1.0f, 1.0f, 0.25f), 4.0f * deltaTime));

      mPosition = Vector3::Lerp(mPosition, pos, 4.0f * deltaTime);
      mRotation = Quaternion::Slerp(
          mRotation, mGame->GetPlayer()->GetRotation(), 4.0f * deltaTime);
      if (mField->GetScale().z < 0.26f) {
        mIsTransitioning = false;
        mField->SetScale(Vector3(1.0f, 1.0f, 0.0f));
        mField->SetOffset(Vector3(0.0f, -0.9f, 0.0f));
        mEdge->SetScale(Vector3(1.0f, 1.0f, 0.25f));
        mEdge->SetOffset(Vector3(0.0f, -0.95f, 0.0f));

        mField->SetVisible(false);
        mEdge->SetVisible(false);

        mPosition = pos;
        mRotation = mGame->GetPlayer()->GetRotation();

        std::cout << "Exited battle mode." << std::endl;
      }
    }
  }
}

unsigned int BattleSystem::GetPlayerPositionNote() {
  return !mInBattle ? 5
                    : mPlayerNotePlayer->GetNoteFromPosition(
                          mGame->GetPlayer()->GetPosition());
}
