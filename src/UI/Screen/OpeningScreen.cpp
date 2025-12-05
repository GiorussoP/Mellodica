//
// Created by rapha on 5/12/2025.
// OpeningScreen.cpp

#include "../../../include/UI/Screen/OpeningScreen.hpp"

#include "Game.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "scenes/MainMenu.hpp"

OpeningScreen::OpeningScreen(class Game *game, const std::string &fontName)
    : UIScreen(game, fontName), mTimer(0.0f) {

  // Add black rectangle background
  auto background = AddImageOrElement(Vector3(0.0f, 0.0f, 0.0f));
  background->SetPosition(Vector3(0.0f, 0.0f, -1.0f));
  background->SetScale(Vector3(3.0f, 3.0f, 1.0f));

  // Add centered text "Abertura Template"
  auto openingText = AddText("Abertura Template", Vector3(1.0f, 1.0f, 1.0f),
                             Vector3(0.0f, 0.0f, 0.0f),
                             0.0f); // White text, transparent background
  openingText->SetPosition(Vector3(0.0f, 0.0f, 0.0f)); // Center
  openingText->SetScale(Vector3(0.5f, 0.5f, 1.0f));    // Scale it appropriately
}

OpeningScreen::~OpeningScreen() { UIScreen::~UIScreen(); }

void OpeningScreen::HandleKeyPress(int key) {
  // On specific key presses, jump to main menu
  if (key == SDLK_RETURN || key == SDLK_SPACE ||
      (key >= SDLK_0 && key <= SDLK_9) || key == SDLK_MINUS ||
      key == SDLK_EQUALS) {
    mGame->LoadScene(new MainMenu(mGame));
  }
}

void OpeningScreen::Update(float deltaTime) {
  UIScreen::Update(deltaTime);
  mTimer += deltaTime;
  if (mTimer >= 28.0f) {
    // Load Main Menu
    mGame->LoadScene(new MainMenu(mGame));
  }
}