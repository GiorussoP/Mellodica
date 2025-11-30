//
// Created by rapha on 29/11/2025.
//

#include "../../../include/UI/Screen/MainMenuScreen.hpp"

#include "Game.hpp"
#include "TestScene.hpp"

MainMenuScreen::MainMenuScreen(class Game *game, const std::string &fontName)
    : UIScreen(game, fontName) {

  auto mB1 = AddButton("assets/textures/hud/iniciar.png",
                       [this] { mGame->LoadScene(new TestSceneB(mGame)); });
  SDL_Log("Button 1 Added!");
  mB1->ButtonSetScale(Vector3(0.5f, 0.5f, 0.0f));
  mB1->ButtonSetPosition(Vector3(0.0f, 0.0f, 0.0f));

  auto mB2 =
      AddButton("assets/textures/hud/sair.png", [this] { mGame->Quit(); });
  SDL_Log("Button 2 Added!");
  mB2->ButtonSetScale(Vector3(0.5f, 0.5f, 0.0f));
  mB2->ButtonSetPosition(Vector3(0.0f, -0.5f, 0.0f));
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
    }
  } else if (key == SDLK_DOWN || key == SDLK_s) {
    // Move to next button
    if (mSelectedButton < static_cast<int>(mHudButtons.size()) - 1) {
      mHudButtons[mSelectedButton]->SetHighlighted(false);
      mSelectedButton++;
      mHudButtons[mSelectedButton]->SetHighlighted(true);
    }
  } else if (key == SDLK_RETURN) {
    // Click the currently selected button
    if (mSelectedButton >= 0 &&
        mSelectedButton < static_cast<int>(mHudButtons.size())) {
      mHudButtons[mSelectedButton]->OnClick();
    }
  }
}