//
// Created by luiza on 09/12/2025.
//

#include "UI/Screen/PauseScreen.hpp"
#include "AssetLoader.hpp"

#include "Game.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "UI/UIButton.hpp"
#include "scenes/MainMenu.hpp"
#include <SDL2/SDL.h>

PauseScreen::PauseScreen(Game *game)
    : UIScreen(game, "fonts/MedodicaRegular.otf") {
  mGame->SetPaused(true);
  MIDIPlayer::pause();

  auto btnResume =
      AddButton(getAssetPath("sprites/scenes/startbutton160x40.png"),
                [this]() { Close(); });

  btnResume->ButtonSetScale(Vector3(0.8f, 0.5f, 0.0f));
  btnResume->ButtonSetPosition(Vector3(0.0f, 0.20f, 0.0f));

  auto btnQuit =
      AddButton(getAssetPath("sprites/scenes/quitbutton160x40.png"), [this]() {
        mGame->SetPaused(false);
        mGame->LoadScene(new MainMenu(mGame));
      });
  btnQuit->ButtonSetScale(Vector3(0.8f, 0.5f, 0.0f));
  btnQuit->ButtonSetPosition(Vector3(0.0f, -0.20f, 0.0f));
}

PauseScreen::~PauseScreen() {
  mGame->SetPaused(false);
  MIDIPlayer::play();
}

void PauseScreen::HandleKeyPress(int key) {
  if (key == SDLK_UP || key == SDLK_w) {
    if (mSelectedButton > 0) {
      mHudButtons[mSelectedButton]->SetHighlighted(false);
      mSelectedButton--;
      mHudButtons[mSelectedButton]->SetHighlighted(true);
      MIDIPlayer::playSequence(
          {{0.0f, 15, 92, true, 120}, {0.05f, 15, 92, false}});
    }
  } else if (key == SDLK_DOWN || key == SDLK_s) {
    if (mSelectedButton < static_cast<int>(mHudButtons.size()) - 1) {
      mHudButtons[mSelectedButton]->SetHighlighted(false);
      mSelectedButton++;
      mHudButtons[mSelectedButton]->SetHighlighted(true);
      MIDIPlayer::playSequence(
          {{0.0f, 15, 92, true, 120}, {0.05f, 15, 92, false}});
    }
  } else if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
    if (mSelectedButton >= 0 &&
        mSelectedButton < static_cast<int>(mHudButtons.size())) {
      mHudButtons[mSelectedButton]->OnClick();
    }
  } else if (key == SDLK_ESCAPE) {
    Close();
  }
}