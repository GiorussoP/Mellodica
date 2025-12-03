#include "PlayerHud.hpp"

PlayerHud::PlayerHud(class Game *game)
    : UIScreen(game, "./assets/fonts/PressStart2P-Regular.ttf"),
      mHPrect(nullptr), mENrect(nullptr), mBarSize(0.52f, 0.09f, 1.0f),
      mBorderSize(0.6f, 0.2f, 1.0f), mLeftBarCenter(-0.7f),
      lastHealth(INFINITY), lastEnergy(INFINITY) {

  // Player energy
  auto energiaBackground = AddImageOrElement(Color::Black);
  energiaBackground->SetPosition(Vector3(mLeftBarCenter, -0.9f, -1.0f));
  energiaBackground->SetScale(mBarSize);
  auto energia =
      AddImageOrElement("./assets/sprites/textures/hud-energia-70x20.png");
  energia->SetPosition(Vector3(mLeftBarCenter, -0.9f, 1.0f));
  energia->SetScale(mBorderSize);
  mENrect = AddImageOrElement(Color::White);
  mENrect->SetPosition(Vector3(mLeftBarCenter, -0.9f, 0.0f));
  mENrect->SetScale(mBarSize);

  // Player health
  auto vidaBackground = AddImageOrElement(Color::Black);
  vidaBackground->SetPosition(Vector3(mLeftBarCenter, -0.7f, -1.0f));
  vidaBackground->SetScale(mBarSize);
  auto vida = AddImageOrElement("./assets/sprites/textures/hud-vida-70x20.png");
  vida->SetPosition(Vector3(mLeftBarCenter, -0.7f, 1.0f));
  vida->SetScale(mBorderSize);
  mHPrect = AddImageOrElement(Color::Red);
  mHPrect->SetPosition(Vector3(mLeftBarCenter, -0.7f, 0.0f));
  mHPrect->SetScale(mBarSize);
}

void PlayerHud::Update(float deltaTime) {

  float newHealth = static_cast<float>(mGame->GetPlayer()->getHealth());

  if (newHealth > lastHealth) {
    mHPrect->GetSpriteComponent().SetColor(Color::Green);
  } else if (newHealth < lastHealth) {
    mHPrect->GetSpriteComponent().SetColor(Vector3(1.0f, 0.5f, 0.0f));
  } else {
    mHPrect->GetSpriteComponent().SetColor(Color::Red);
  }
  lastHealth = static_cast<int>(newHealth);

  // Player health bar
  mHPrect->SetScale(Vector3(
      mBarSize.x *
          (newHealth / static_cast<float>(mGame->GetPlayer()->getMaxHealth())),
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
}