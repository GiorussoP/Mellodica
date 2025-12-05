//
// Created by rapha on 28/11/2025.
//

#ifndef MELLODICA_UISCREEN_H
#define MELLODICA_UISCREEN_H

#include "../HUDElement.hpp"
#include "../TextElement.hpp"
#include "../UIButton.hpp"
#include "Game.hpp"
#include "Renderer.hpp"

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
  HUDElement *AddImageOrElement(const Vector3 &color) {
    auto hE = new HUDElement(mGame);
    hE->GetSpriteComponent().SetColor(color);
    hE->GetSpriteComponent().SetTextureIndex(-1); // No texture, just color
    mHudImages.push_back(hE);
    return hE;
  }

  // Text elements add
  TextElement *AddText(const std::string &text, const Vector3 &color,
                       const Vector3 &bgColor, float bgAlpha = 1.0f) {
    auto textElem = new TextElement(mGame, text, color, bgColor, bgAlpha);
    mTextElements.push_back(textElem);
    return textElem;
  } // UI Buttons add
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
  std::vector<TextElement *> mTextElements;
};

#endif // MELLODICA_UISCREEN_H