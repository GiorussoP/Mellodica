#include "PlayerHud.hpp"

PlayerHud::PlayerHud(class Game *game)
    : UIScreen(game, "./assets/fonts/PressStart2P-Regular.ttf"),
      mHPrect(nullptr), mENrect(nullptr), mBarSize(0.52f, 0.09f, 1.0f),
      mBorderSize(0.6f, 0.2f, 1.0f), mLeftBarCenter(-0.7f) {
  // Add HUD elements here, e.g., health bar, energy bar, etc.

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
}

void PlayerHud::Update(float deltaTime) {
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
}