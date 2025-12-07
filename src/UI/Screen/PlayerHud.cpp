#include "UI/Screen/PlayerHud.hpp"

PlayerHud::PlayerHud(class Game *game)
    : UIScreen(game, "./assets/fonts/MedodicaRegular.otf"), mHPrect(nullptr),
      mENrect(nullptr), mHPText(nullptr), mENText(nullptr),
      mBarSize(0.52f, 0.09f, 1.0f), mBorderSize(0.6f, 0.2f, 1.0f),
      mLeftBarCenter(-0.7f), lastHealth(500), lastEnergy(200) {

  // Player energy
  auto energiaBackground = AddImageOrElement(Color::Black);
  energiaBackground->SetPosition(Vector3(mLeftBarCenter, -0.9f, -1.0f));
  energiaBackground->SetScale(mBarSize);
  auto energia =
      AddImageOrElement("./assets/sprites/textures/hud-energia-70x20.png");
  energia->SetPosition(Vector3(mLeftBarCenter, -0.9f, 1.0f));
  energia->SetScale(mBorderSize);
  mENrect = AddImageOrElement(Color::DarkGray);
  mENrect->SetPosition(Vector3(mLeftBarCenter, -0.9f, 0.0f));
  mENrect->SetScale(mBarSize);

  // Player health
  auto vidaBackground = AddImageOrElement(Color::Black);
  vidaBackground->SetPosition(Vector3(mLeftBarCenter, -0.7f, -1.0f));
  vidaBackground->SetScale(mBarSize);
  auto vida = AddImageOrElement("./assets/sprites/textures/hud-vida-70x20.png");
  vida->SetPosition(Vector3(mLeftBarCenter, -0.7f, 1.0f));
  vida->SetScale(mBorderSize);
  mHPrect = AddImageOrElement(Color::White);
  mHPrect->SetPosition(Vector3(mLeftBarCenter, -0.7f, 0.0f));
  mHPrect->SetScale(mBarSize);

  // HP counter text
  mHPText = AddText("HP: 100/100", Color::Gray, Color::Black, 0.0f);
  mHPText->SetPosition(Vector3(mLeftBarCenter, -0.7f, 1.0f));
  mHPText->SetScale(Vector3(0.25f, 0.08f, 1.0f));

  // Energy counter text
  mENText = AddText("EN: 100/100", Color::White, Color::Black, 0.0f);
  mENText->SetPosition(Vector3(mLeftBarCenter, -0.91f, 1.0f));
  mENText->SetScale(Vector3(0.25f, 0.08f, 1.0f));
}

void PlayerHud::Update(float deltaTime) {

  float targetHealth = static_cast<float>(mGame->GetPlayer()->getHealth());
  int newHealth = std::round(Math::Lerp(static_cast<float>(lastHealth),
                                        targetHealth, deltaTime * 10.0f));

  if (newHealth > lastHealth) {
    mHPrect->GetSpriteComponent().SetColor(Color::Green);
  } else if (newHealth < lastHealth) {
    mHPrect->GetSpriteComponent().SetColor(Vector3(1.0f, 0.5f, 0.0f));
  } else {
    mHPrect->GetSpriteComponent().SetColor(Color::White);
  }
  lastHealth = newHealth;

  // Player health bar
  mHPrect->SetScale(Vector3(
      (mBarSize.x - 0.09) *
          (newHealth / static_cast<float>(mGame->GetPlayer()->getMaxHealth())),
      mBarSize.y, mBarSize.z));

  mHPrect->SetPosition(mHPrect->GetPosition() +
                       Vector3(-mHPrect->GetPosition().x + 0.09f +
                                   mLeftBarCenter -
                                   (mBarSize.x - mHPrect->GetScale().x) / 2.0f,
                               0.0f, 0.0f));

  // Player energy bar
  mENrect->SetScale(
      Vector3((mBarSize.x - 0.07) * (mGame->GetPlayer()->getEnergy() /
                                     mGame->GetPlayer()->getMaxEnergy()),
              mBarSize.y, mBarSize.z));

  mENrect->SetPosition(mENrect->GetPosition() +
                       Vector3(-mENrect->GetPosition().x + 0.07f +
                                   mLeftBarCenter -
                                   (mBarSize.x - mENrect->GetScale().x) / 2.0f,
                               0.0f, 0.0f));

  // Update HP text
  int currentHP = mGame->GetPlayer()->getHealth();
  int maxHP = mGame->GetPlayer()->getMaxHealth();
  mHPText->SetText("HP: " + std::to_string(currentHP) + "/" +
                   std::to_string(maxHP));

  // Update Energy text
  int currentEN = static_cast<int>(mGame->GetPlayer()->getEnergy());
  int maxEN = static_cast<int>(mGame->GetPlayer()->getMaxEnergy());
  mENText->SetText("EN: " + std::to_string(currentEN) + "/" +
                   std::to_string(maxEN));
}
