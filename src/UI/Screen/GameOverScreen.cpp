//
// Created by rapha on 5/12/2025.
// GameOverScreen.cpp

#include "../../../include/UI/Screen/GameOverScreen.hpp"

#include "Game.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "scenes/MainMenu.hpp"

GameOverScreen::GameOverScreen(class Game *game, const std::string &fontName)
    : UIScreen(game, fontName) {

  // Add black rectangle background
  auto background = AddImageOrElement(Color::Black);
  background->SetPosition(Vector3(0.0f, 0.0f, -1.0f));
  background->SetScale(Vector3(3.0f, 3.0f, 1.0f));

  // Add centered red text "Fim de jogo"
  mGame->GetRenderer()->SetIsDark(true);
  auto gameOverText =
      AddText("FIM DE JOGO", Color::Red, Vector3(0.0f, 0.0f, 0.0f), 0.0f);
  gameOverText->SetPosition(Vector3(0.0f, 0.1f, 0.0f));
  gameOverText->SetScale(Vector3(0.5f, 0.5f, 1.0f));

  auto instructionText =
      AddText("Pressione Enter para voltar ao menu principal", Color::White,
              Vector3(0.0f, 0.3f, 0.0f), 0.0f);
  instructionText->SetPosition(Vector3(0.0f, -0.3f, 0.0f));
  instructionText->SetScale(Vector3(1.5f, 0.2f, 1.0f));
}

GameOverScreen::~GameOverScreen() { UIScreen::~UIScreen(); }

void GameOverScreen::HandleKeyPress(int key) {
  if (key == SDLK_RETURN) {
    // Go back to main menu
    mGame->LoadScene(new MainMenu(mGame));
  }
}