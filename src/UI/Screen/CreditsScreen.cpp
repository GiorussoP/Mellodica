//
// Created by rapha on 5/12/2025.
// CreditsScreen.cpp

#include "../../../include/UI/Screen/CreditsScreen.hpp"

#include "Game.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "actors/SceneActors.hpp"
#include "scenes/MainMenu.hpp"

CreditsScreen::CreditsScreen(class Game *game, const std::string &fontName)
    : UIScreen(game, fontName), mTimer(0.0f) {

  mGame->GetRenderer()->SetIsDark(true);
  mGame->GetRenderer()->setNight();

  // Add credits text
  mCreditsText = AddText(
      "MELLODICA\n\n\n\n\nGiovanni Russo Paschoal:\n Engine, MIDI "
      "Programming, Shaders\n\n\n\n"
      "Luis Henrique Gomes Higino:\n Engine, Algorithms & Mechanics\n\n\n\n"
      "Luisa Lopes Carvalhães:\n 2D Art, Animations, Assets\n\n\n\n"
      "Luiza Sodre Salgado\n Items, Puzzles & Mechanics\n\n\n\n"
      "Raphael Aroldo Carreiro Mendes:\n HUD, Menus & Interfaces\n\n\n\n"
      "Soundtrack by:\n jennix\n\n\n\n"
      "Special thanks to:\n Lucas N. Ferreira\n\n\n\n\nThanks "
      "for playing!",
      Color::Yellow, Vector3(0.0f, 0.0f, 0.0f), 0.0f);
  mCreditsText->SetPosition(Vector3(0.35f, -3.0f, 0.0f)); // Start below screen
  mCreditsText->SetScale(Vector3(1.2f, 4.0f, 1.0f));

  mNotePlayer = new NotePlayerActor(mGame);
  mNotePlayer->SetPosition(Vector3(-6.25f, 0.0f, 5.0f));

  // Look straight down
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

  // DO NOTHING, players could accidentally skip credits

  /*
  if (key == SDLK_RETURN || key == SDLK_SPACE ||
      (key >= SDLK_0 && key <= SDLK_9) || key == SDLK_MINUS ||
      key == SDLK_EQUALS) {
    // Reset save to level0 after viewing credits
    mGame->ResetSaveToLevel0();
    mGame->LoadScene(new MainMenu(mGame));
  }
    */
}

void CreditsScreen::Update(float deltaTime) {
  UIScreen::Update(deltaTime);
  mTimer += deltaTime;

  // Move text up
  Vector3 pos = mCreditsText->GetPosition();
  pos.y += deltaTime * (8.0f / 100.0f); // Adjust speed
  mCreditsText->SetPosition(pos);

  // After 30 seconds or when text is off screen, go to main menu
  if (mTimer >= 88.0f) {
    // Reset save to level0 after viewing credits
    mGame->ResetSaveToLevel0();
    mGame->LoadScene(new MainMenu(mGame));
  }

  auto events = MIDIPlayer::pollNoteEvents();

  for (auto &event : events) {
    if (event.channel >= 2 && event.channel != 9) {
      if (event.noteOn) {
        mNotePlayer->PlayNote(event.note, event.channel, true, 2.0f);
      } else {
        mNotePlayer->EndNote(event.note);
      }
    }
  }
}
