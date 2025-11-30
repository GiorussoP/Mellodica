
#include "BattleScreen.hpp"
#include "BattleSystem.hpp"
#include "EnemyGroup.hpp"

BattleScreen::BattleScreen(class Game *game, const std::string &fontName)
    : UIScreen(game, fontName), mBarSize(0.52f, 0.09f, 1.0f),
      mBorderSize(0.6f, 0.2f, 1.0f), mLeftBarCenter(-0.7f),
      mRightBarCenter(0.7f) {

  // Player energy
  auto energia =
      AddImageOrElement("./assets/sprites/textures/hud-energia-70x20.png");
  energia->SetPosition(Vector3(mLeftBarCenter, -0.9f, 1.0f));
  energia->SetScale(mBorderSize);
  mENrect = AddImageOrElement(Color::White);
  mENrect->SetPosition(Vector3(mLeftBarCenter, -0.9f, 0.0f));
  mENrect->SetScale(mBarSize);

  // Player health
  auto vida = AddImageOrElement("./assets/sprites/textures/hud-vida-70x20.png");
  vida->SetPosition(Vector3(mLeftBarCenter, -0.7f, 1.0f));
  vida->SetScale(mBorderSize);
  mHPrect = AddImageOrElement(Color::Red);
  mHPrect->SetPosition(Vector3(mLeftBarCenter, -0.7f, 0.0f));
  mHPrect->SetScale(mBarSize);

  // Enemy health bars
  for (auto &enemy :
       game->GetBattleSystem()->GetCurrentEnemyGroup()->GetEnemies()) {
    auto enemyHPBorder =
        AddImageOrElement("./assets/sprites/textures/hud-vida-70x20.png");
    enemyHPBorder->SetPosition(
        Vector3(mRightBarCenter,
                0.9f - static_cast<float>(mEnemyHPrects.size()) * 0.2f, 1.0f));
    enemyHPBorder->SetScale(mBorderSize);
    enemyHPBorder->GetSpriteComponent().SetColor(
        NOTE_COLORS[enemy->GetChannel()] * 2.0f);

    auto enemyHPRect = AddImageOrElement(NOTE_COLORS[enemy->GetChannel()]);
    enemyHPRect->SetPosition(
        Vector3(mRightBarCenter,
                0.9f - static_cast<float>(mEnemyHPrects.size()) * 0.2f, 0.0f));
    enemyHPRect->SetScale(mBarSize);

    mEnemyHPrects.push_back(enemyHPRect);
  }

  // Ally health bars
  for (auto &ally : game->GetPlayer()->GetActiveAllies()) {
    auto allyHPBorder =
        AddImageOrElement("./assets/sprites/textures/hud-vida-70x20.png");
    allyHPBorder->SetPosition(
        Vector3(mLeftBarCenter,
                -0.5f + static_cast<float>(mAllyHPrects.size()) * 0.2f, 1.0f));
    allyHPBorder->SetScale(mBorderSize);
    allyHPBorder->GetSpriteComponent().SetColor(
        NOTE_COLORS[ally->GetChannel()] * 2.0f);

    auto allyHPRect = AddImageOrElement(NOTE_COLORS[ally->GetChannel()]);
    allyHPRect->SetPosition(
        Vector3(mLeftBarCenter,
                -0.5f + static_cast<float>(mAllyHPrects.size()) * 0.2f, 0.0f));
    allyHPRect->SetScale(mBarSize);

    mAllyHPrects.push_back(allyHPRect);
  }
}

BattleScreen::~BattleScreen() { UIScreen::~UIScreen(); }

void BattleScreen::Update(float deltaTime) {

  // Player health bar
  mHPrect->SetScale(Vector3(
      mBarSize.x * (static_cast<float>(mGame->GetPlayer()->getHealth()) /
                    static_cast<float>(mGame->GetPlayer()->getMaxHealth())),
      mBarSize.y, mBarSize.z));

  mHPrect->SetPosition(mHPrect->GetPosition() +
                       Vector3(-mHPrect->GetPosition().x + mLeftBarCenter -
                                   (mBarSize.x - mHPrect->GetScale().x) / 2.0f,
                               0.0f, 0.0f));

  // Player energy bar
  mENrect->SetScale(Vector3(mBarSize.x * (mGame->GetPlayer()->getEnergy() /
                                          mGame->GetPlayer()->getMaxEnergy()),
                            mBarSize.y, mBarSize.z));

  mENrect->SetPosition(mENrect->GetPosition() +
                       Vector3(-mENrect->GetPosition().x + mLeftBarCenter -
                                   (mBarSize.x - mENrect->GetScale().x) / 2.0f,
                               0.0f, 0.0f));

  // Enemy health bars
  auto &enemies =
      mGame->GetBattleSystem()->GetCurrentEnemyGroup()->GetEnemies();
  for (size_t i = 0; i < enemies.size(); ++i) {
    mEnemyHPrects[i]->SetScale(
        Vector3(mBarSize.x * (static_cast<float>(enemies[i]->GetHealth()) /
                              static_cast<float>(enemies[i]->GetMaxHealth())),
                mBarSize.y, mBarSize.z));
    mEnemyHPrects[i]->SetPosition(
        mEnemyHPrects[i]->GetPosition() +
        Vector3(-mEnemyHPrects[i]->GetPosition().x + mRightBarCenter -
                    (mBarSize.x - mEnemyHPrects[i]->GetScale().x) / 2.0f,
                0.0f, 0.0f));
  }

  // Ally health bars
  auto &allies = mGame->GetPlayer()->GetActiveAllies();
  for (size_t i = 0; i < allies.size(); ++i) {
    mAllyHPrects[i]->SetScale(
        Vector3(mBarSize.x * (static_cast<float>(allies[i]->GetHealth()) /
                              static_cast<float>(allies[i]->GetMaxHealth())),
                mBarSize.y, mBarSize.z));
    mAllyHPrects[i]->SetPosition(
        mAllyHPrects[i]->GetPosition() +
        Vector3(-mAllyHPrects[i]->GetPosition().x + mLeftBarCenter -
                    (mBarSize.x - mAllyHPrects[i]->GetScale().x) / 2.0f,
                0.0f, 0.0f));
  }
}