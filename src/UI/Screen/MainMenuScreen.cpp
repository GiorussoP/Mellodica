//
// Created by rapha on 29/11/2025.
//

#include "../../../include/UI/Screen/MainMenuScreen.hpp"

#include "AssetLoader.hpp"
#include "Game.hpp"
#include "scenes/Level0.hpp"
#include "scenes/Level1.hpp"
#include "scenes/Level2.hpp"
#include "scenes/Level3.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "scenes/CreditsScene.hpp"

MainMenuScreen::MainMenuScreen(class Game *game, const std::string &fontName)
    : UIScreen(game, fontName) {

  auto mB1 = AddButton(getAssetPath("textures/hud/iniciar.png"), [this] {
    // Load saved state and go to saved level
    auto savedState = mGame->LoadState();
    int savedScene = 0; // Default to level0 (which means start from level1)
    if (savedState.find("SCENE_STATE") != savedState.end()) {
      savedScene = savedState["SCENE_STATE"];
    }

    // Load the appropriate level
    // Note: scene0 means start from beginning (Level1)
    switch (savedScene) {
    case 0:
      mGame->LoadScene(new Level0(mGame));
      break;
    case 1:
      mGame->LoadScene(new Level1(mGame));
      break;
    case 2:
      mGame->LoadScene(new Level2(mGame));
      break;
    case 3:
      mGame->LoadScene(new Level3(mGame));
      break;
    default:
      mGame->LoadScene(new Level1(mGame));
      break;
    }
  });
  SDL_Log("Button 1 Added!");
  mB1->ButtonSetScale(Vector3(0.5f, 0.5f, 0.0f));
  mB1->ButtonSetPosition(Vector3(0.0f, 0.25f, 0.0f));

  auto mB2 =
      AddButton(getAssetPath("textures/hud/sair.png"), [this] { mGame->Quit(); });
  SDL_Log("Button 2 Added!");
  mB2->ButtonSetScale(Vector3(0.5f, 0.5f, 0.0f));
  mB2->ButtonSetPosition(Vector3(0.0f, -0.2f, 0.0f));

  auto image = AddImageOrElement(getAssetPath("sprites/scenes/title-screen.png"));
  image->SetPosition(Vector3(0.0f, 0.0f, -1.0f));
  image->SetScale(Vector3(2.0f, 2.0f, 1.0f));
}

MainMenuScreen::~MainMenuScreen() { UIScreen::~UIScreen(); }

void MainMenuScreen::HandleKeyPress(int key) {
  // Navigate between buttons with UP/DOWN arrows
  // SDL_Log("Executing Main Menu HandleKeyPress!");
  if (key == SDLK_UP || key == SDLK_w) {
    // Move to previous button
    if (mSelectedButton > 0) {
      mHudButtons[mSelectedButton]->SetHighlighted(false);
      mSelectedButton--;
      mHudButtons[mSelectedButton]->SetHighlighted(true);

      MIDIPlayer::playSequence(
          {{0.0f, 15, 92, true, 120}, {0.05f, 15, 92, false}});
    }
  } else if (key == SDLK_DOWN || key == SDLK_s) {
    // Move to next button
    if (mSelectedButton < static_cast<int>(mHudButtons.size()) - 1) {
      mHudButtons[mSelectedButton]->SetHighlighted(false);
      mSelectedButton++;
      mHudButtons[mSelectedButton]->SetHighlighted(true);

      MIDIPlayer::playSequence(
          {{0.0f, 15, 92, true, 120}, {0.05f, 15, 92, false}});
    }
  } else if (key == SDLK_RETURN) {
    // Click the currently selected button
    if (mSelectedButton >= 0 &&
        mSelectedButton < static_cast<int>(mHudButtons.size())) {
      mHudButtons[mSelectedButton]->OnClick();

      MIDIPlayer::playSequence({{0.0f, 14, 62, true, 100},
                                {0.0f, 14, 66, true, 80},
                                {0.2f, 14, 62, false},
                                {0.0f, 14, 66, false},
                                {0.0f, 14, 69, true, 100},
                                {0.0f, 14, 74, true, 80},
                                {0.2f, 14, 69, false},
                                {0.0f, 14, 74, false},
                                {0.0f, 14, 78, true, 120},
                                {0.4f, 14, 78, false}});
    }
  }
}
