//
// Created by rapha on 28/11/2025.
//

#include "../../../include/UI/Screen/UIScreen.hpp"
#include "../../../include/Game.hpp"
#include "../../../include/UI/HUDElement.hpp"
#include "../../../include/UI/UIButton.hpp"
#include "Renderer.hpp"

UIScreen::UIScreen(class Game *game, const std::string &fontName)
    : mGame(game), mState(UIState::Active), mSelectedButton(-1) {
  mGame->PushUI(this);
  SDL_Log("UIScreen::UIScreen - Added UI to the Game UI Stack");
}

UIScreen::~UIScreen() {
  for (auto img : mHudImages) {
    mGame->RemoveActor(img);
    mGame->GetRenderer()->RemoveUIElement(img);
  }
  mHudImages.clear();
  for (auto button : mHudButtons) {
    mGame->RemoveActor(button);
    mGame->GetRenderer()->RemoveUIElement(button);
  }
  mHudButtons.clear();
  for (auto text : mTextElements) {
    mGame->RemoveActor(text);
  }
  mTextElements.clear();
}

void UIScreen::Update(float deltaTime) {}

void UIScreen::HandleKeyPress(int key) {}

UIButton *UIScreen::AddButton(const std::string &hudTexturePath,
                              const std::string &hudAtlasPath,
                              std::function<void()> onClick) {
  auto hB = new UIButton(mGame, onClick, hudTexturePath, hudAtlasPath);
  mHudButtons.emplace_back(hB);

  if (mHudButtons.size() == 1) {
    mSelectedButton = 0;
    hB->SetHighlighted(true);
  }

  return hB;
}

UIButton *UIScreen::AddButton(const std::string &singleImagePath,
                              std::function<void()> onClick) {
  auto hB = new UIButton(mGame, onClick, singleImagePath);
  mHudButtons.emplace_back(hB);

  if (mHudButtons.size() == 1) {
    mSelectedButton = 0;
    hB->SetHighlighted(true);
  }

  return hB;
}
