
#include "BattleScreen.hpp"
#include "BattleSystem.hpp"
#include "EnemyGroup.hpp"

BattleScreen::BattleScreen(class Game *game, const std::string &fontName)
    : UIScreen(game, fontName), mBarSize(0.52f, 0.09f, 1.0f),
      mBorderSize(0.6f, 0.2f, 1.0f), mLeftBarCenter(-0.7f),
      mRightBarCenter(0.7f) {

  // Enemy health bars
  for (auto &enemy :
       game->GetBattleSystem()->GetCurrentEnemyGroup()->GetEnemies()) {
    if (!enemy || enemy->GetState() == ActorState::Destroy) continue;
    mEnemyLastHealths.push_back(enemy->GetHealth());

    auto enemyHPBackground = AddImageOrElement(Color::Black);
    enemyHPBackground->SetPosition(
        Vector3(mRightBarCenter,
                0.9f - static_cast<float>(mEnemyHPrects.size()) * 0.2f, -1.0f));
    enemyHPBackground->SetScale(mBarSize);

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
        Vector3(mRightBarCenter + 0.09f,
                0.9f - static_cast<float>(mEnemyHPrects.size()) * 0.2f, 0.0f));
    enemyHPRect->SetScale(mBarSize - Vector3(0.09f, 0.0f, 0.0f));

    mEnemyHPrects.push_back(enemyHPRect);

    // Enemy HP text counter
    auto enemyHPText =
        AddText("HP: 100/100", NOTE_COLORS[enemy->GetChannel()] * 0.5f,
                Color::Black, 0.0f);
    enemyHPText->SetPosition(
        Vector3(mRightBarCenter,
                0.9f - static_cast<float>(mEnemyHPTexts.size()) * 0.2f, 2.0f));
    enemyHPText->SetScale(Vector3(0.25f, 0.08f, 1.0f));
    mEnemyHPTexts.push_back(enemyHPText);
  }

  // Ally health bars
  for (auto &ally : game->GetPlayer()->GetActiveAllies()) {
    if (!ally || ally->GetState() == ActorState::Destroy) continue;
    mAllyLastHealths.push_back(ally->GetHealth());

    auto allyHPBackground = AddImageOrElement(Color::Black);
    allyHPBackground->SetPosition(
        Vector3(mLeftBarCenter,
                -0.5f + static_cast<float>(mAllyHPrects.size()) * 0.2f, -1.0f));
    allyHPBackground->SetScale(mBarSize);

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
        Vector3(mLeftBarCenter + 0.09f,
                -0.5f + static_cast<float>(mAllyHPrects.size()) * 0.2f, 0.0f));
    allyHPRect->SetScale(mBarSize - Vector3(0.09f, 0.0f, 0.0f));

    mAllyHPrects.push_back(allyHPRect);

    // Ally HP text counter
    auto allyHPText =
        AddText("HP: 100/100", NOTE_COLORS[ally->GetChannel()] * 0.5f,
                Color::Black, 0.0f);
    allyHPText->SetPosition(
        Vector3(mLeftBarCenter,
                -0.5f + static_cast<float>(mAllyHPTexts.size()) * 0.2f, 2.0f));
    allyHPText->SetScale(Vector3(0.25f, 0.08f, 1.0f));
    mAllyHPTexts.push_back(allyHPText);
  }
}

BattleScreen::~BattleScreen() {}

void BattleScreen::Update(float deltaTime) {
  // Check if battle has ended (enemy group is null)
  auto *enemyGroup = mGame->GetBattleSystem()->GetCurrentEnemyGroup();
  if (!enemyGroup) {
    return; // Battle ended, skip update
  }

  // Enemy health bars
  auto &enemies = enemyGroup->GetEnemies();
  for (size_t i = 0; i < enemies.size(); ++i) {

    if (mEnemyLastHealths[i] > enemies[i]->GetHealth()) {
      mEnemyHPrects[i]->GetSpriteComponent().SetColor(
          Vector3(1.0f, 0.5f, 0.0f));
    } else if (mEnemyLastHealths[i] < enemies[i]->GetHealth()) {
      mEnemyHPrects[i]->GetSpriteComponent().SetColor(Color::Green);
    } else {
      mEnemyHPrects[i]->GetSpriteComponent().SetColor(
          NOTE_COLORS[enemies[i]->GetChannel()]);
    }
    mEnemyLastHealths[i] = enemies[i]->GetHealth();

    mEnemyHPrects[i]->SetScale(Vector3(
        (mBarSize.x - 0.09f) * (static_cast<float>(enemies[i]->GetHealth()) /
                                static_cast<float>(enemies[i]->GetMaxHealth())),
        mBarSize.y, mBarSize.z));
    mEnemyHPrects[i]->SetPosition(
        mEnemyHPrects[i]->GetPosition() +
        Vector3(-mEnemyHPrects[i]->GetPosition().x + 0.09f + mRightBarCenter -
                    (mBarSize.x - mEnemyHPrects[i]->GetScale().x) / 2.0f,
                0.0f, 0.0f));

    // Update enemy HP text
    int currentHP = enemies[i]->GetHealth();
    int maxHP = enemies[i]->GetMaxHealth();
    mEnemyHPTexts[i]->SetText("HP: " + std::to_string(currentHP) + "/" +
                              std::to_string(maxHP));
  }

  // Ally health bars
  auto &allies = mGame->GetPlayer()->GetActiveAllies();
  for (size_t i = 0; i < allies.size(); ++i) {

    if (mAllyLastHealths[i] > allies[i]->GetHealth()) {
      mAllyHPrects[i]->GetSpriteComponent().SetColor(Vector3(1.0f, 0.5f, 0.0f));
    } else if (mAllyLastHealths[i] < allies[i]->GetHealth()) {
      mAllyHPrects[i]->GetSpriteComponent().SetColor(Color::Green);
    } else {
      mAllyHPrects[i]->GetSpriteComponent().SetColor(
          NOTE_COLORS[allies[i]->GetChannel()]);
    }
    mAllyLastHealths[i] = allies[i]->GetHealth();

    mAllyHPrects[i]->SetScale(Vector3(
        (mBarSize.x - 0.09f) * (static_cast<float>(allies[i]->GetHealth()) /
                                static_cast<float>(allies[i]->GetMaxHealth())),
        mBarSize.y, mBarSize.z));
    mAllyHPrects[i]->SetPosition(
        mAllyHPrects[i]->GetPosition() +
        Vector3(-mAllyHPrects[i]->GetPosition().x + 0.09f + mLeftBarCenter -
                    (mBarSize.x - mAllyHPrects[i]->GetScale().x) / 2.0f,
                0.0f, 0.0f));

    // Update ally HP text
    int currentHP = allies[i]->GetHealth();
    int maxHP = allies[i]->GetMaxHealth();
    mAllyHPTexts[i]->SetText("HP: " + std::to_string(currentHP) + "/" +
                             std::to_string(maxHP));
  }
}