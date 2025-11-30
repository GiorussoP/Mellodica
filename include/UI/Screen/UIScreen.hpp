//
// Created by rapha on 28/11/2025.
//

#ifndef MELLODICA_UISCREEN_H
#define MELLODICA_UISCREEN_H

#include "../HUDElement.hpp"
#include "../UIButton.hpp"

class UIScreen {
public:
  enum class UIState { Active, Closing };

  UIScreen(class Game *game, const std::string &fontName);

  void Close() { mState = UIState::Closing; }

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

  // UI Buttons add
  UIButton *AddButton(const std::string &hudTexturePath,
                      const std::string &hudAtlasPath,
                      std::function<void()> onClick);

  UIButton *AddButton(const std::string &singleImagePath,
                      std::function<void()> onClick);

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