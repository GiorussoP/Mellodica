//
// Created by rapha on 5/12/2025.
// CreditsScreen.cpp

#include "../../../include/UI/Screen/CreditsScreen.hpp"

#include "Game.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "TestActors.hpp"
#include "scenes/MainMenu.hpp"

CreditsScreen::CreditsScreen(class Game *game, const std::string &fontName)
    : UIScreen(game, fontName), mTimer(0.0f) {

  mGame->GetRenderer()->SetIsDark(true);
  mGame->GetRenderer()->setNight();

  // Add credits text
  mCreditsText = AddText(
      "MELLODICA\n\n\n\n\nGiovanni Russo Paschoal:\n Engine, Programação "
      "MIDI, Shaders\n\n\n\n"
      "Luis Henrique Gomes Higino:\n Engine, Algoritmos e Mecânicas\n\n\n\n"
      "Luisa Lopes Carvalhães:\n Arte 2D, Animações, Assets\n\n\n\n"
      "Luiza Sodre Salgado\n Itens, Puzzles e Mecânicas\n\n\n\n"
      "Raphael Aroldo Carreiro Mendes:\n HUD, menus e interfaces\n\n\n\n"
      "Trilha sonora por:\n jennix\n\n\n\n"
      "Agradecimentos especiais:\n Lucas N. Ferreira\n\n\n\n\nObrigado "
      "por jogar!",
      Color::Yellow, Vector3(0.0f, 0.0f, 0.0f), 0.0f);
  mCreditsText->SetPosition(Vector3(0.35f, -3.0f, 0.0f)); // Start below screen
  mCreditsText->SetScale(Vector3(1.2f, 4.0f, 1.0f));

  mNotePlayer = new NotePlayerActor(mGame);
  mNotePlayer->SetPosition(Vector3(-6.25f, 0.0f, 5.0f));
  // Rotate 90 degrees around Y axis so notes shoot in +X direction (across
  // screen)
  mNotePlayer->SetRotation(Math::LookRotation(Vector3::NegUnitZ));

  mGame->GetCamera()->SetPosition(Vector3(0.0f, 1.0f, 0.0f));
  // Look straight down
  mGame->GetCamera()->SetRotation(
      Quaternion(Vector3::UnitX, Math::ToRadians(-90.0f)));
  mGame->GetCamera()->SetMode(CameraMode::Fixed);
}

CreditsScreen::~CreditsScreen() { UIScreen::~UIScreen(); }

void CreditsScreen::HandleKeyPress(int key) {
  // On specific key presses, jump to main menu
  if (key == SDLK_RETURN || key == SDLK_SPACE ||
      (key >= SDLK_0 && key <= SDLK_9) || key == SDLK_MINUS ||
      key == SDLK_EQUALS) {
    mGame->LoadScene(new MainMenu(mGame));
  }
}

void CreditsScreen::Update(float deltaTime) {
  UIScreen::Update(deltaTime);
  mTimer += deltaTime;

  // Move text up
  Vector3 pos = mCreditsText->GetPosition();
  pos.y += deltaTime * (8.0f / 100.0f); // Adjust speed
  mCreditsText->SetPosition(pos);

  // After 30 seconds or when text is off screen, go to main menu
  if (mTimer >= 90.0f) {
    mGame->LoadScene(new MainMenu(mGame));
  }

  auto events = MIDIPlayer::pollNoteEvents();

  for (auto &event : events) {
    if (event.noteOn) {
      mNotePlayer->PlayNote(event.note, event.channel, true, 2.0f);
    } else {
      mNotePlayer->EndNote(event.note);
    }
  }
}