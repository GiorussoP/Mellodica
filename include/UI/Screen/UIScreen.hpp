//
// Created by rapha on 28/11/2025.
//

#ifndef MELLODICA_UISCREEN_H
#define MELLODICA_UISCREEN_H

#include "../HUDElement.hpp"
#include "../UIButton.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

class UIScreen {
public:
  enum class UIState { Active, Closing };

  UIScreen(class Game *game, const std::string &fontName);

  void Close() {
    if (mState == UIState::Closing) {
      return;
    }
    mState = UIState::Closing;
    for (auto img : mHudImages) {
      // button->SetState(ActorState::Destroy);
      mGame->RemoveActor(img);
      mGame->GetRenderer()->RemoveUIElement(img);
      // delete img;
    }
    for (auto button : mHudButtons) {
      // button->SetState(ActorState::Destroy);
      mGame->RemoveActor(button);
      mGame->GetRenderer()->RemoveUIElement(button);
      // delete button;
    }
    mHudImages.clear();
    mHudButtons.clear();
  }

  // HUDElements add
  HUDElement *AddImageOrElement(const std::string &hudTexturePath,
                                const std::string &hudAtlasPath) {
    auto hE = new HUDElement(mGame, hudTexturePath, hudAtlasPath);
    mHudImages.push_back(hE);
    return hE;
  }

  HUDElement *AddImageOrElement(const std::string &singleImagePath) {
    auto hE = new HUDElement(mGame, singleImagePath);
    mHudImages.push_back(hE);
    return hE;
  }
  HUDElement *AddImageOrElement(const Vector3 &color) {
    auto hE = new HUDElement(mGame);
    hE->GetSpriteComponent().SetColor(color);
    hE->GetSpriteComponent().SetTextureIndex(-1); // No texture, just color
    mHudImages.push_back(hE);
    return hE;
  }

  // UI Buttons add
  UIButton *AddButton(const std::string &hudTexturePath,
                      const std::string &hudAtlasPath,
                      std::function<void()> onClick);

  UIButton *AddButton(const std::string &singleImagePath,
                      std::function<void()> onClick);

  UIScreen::UIState GetUIState() const { return mState; }

  virtual ~UIScreen();

  virtual void Update(float deltaTime);
  virtual void HandleKeyPress(int key);

protected:
  class Game *mGame;
  // class Font* mFont;

  UIState mState;

  int mSelectedButton;
  std::vector<HUDElement *> mHudImages;
  std::vector<UIButton *> mHudButtons;
};

#endif // MELLODICA_UISCREEN_H