//
// Created by Lucas N. Ferreira on 22/05/25.
//

#pragma once

#include "../Math.hpp"
#include "HUDElement.hpp"
#include <functional>
#include <string>

class UIButton : public HUDElement {
public:
  UIButton(class Game *game, std::function<void()> onClick,
           const std::string &hudTexturePath, const std::string &hudAtlasPath);

  UIButton(class Game *game, std::function<void()> onClick,
           const std::string &singleImagePath);

  ~UIButton();

  void SetHighlighted(bool sel) {

    mHighlighted = sel;

    if (mHighlighted) {
      mAuxiliarScale.x = mScale.x;
      mAuxiliarScale.y = mScale.y;
      mAuxiliarScale.z = mScale.z;
      mScale *= 2;
    } else {
      mScale.x = mAuxiliarScale.x;
      mScale.y = mAuxiliarScale.y;
      mScale.z = mAuxiliarScale.z;
    }

    GetSpriteComponent().SetScale(mScale);
  }

  bool GetHighlighted() const { return mHighlighted; }
  /*
  void SetAuxiliarScale() {
      mAuxiliarScale = new Vector3(mScale.x, mScale.y, mScale.z);
  }*/

  // Called when button is clicked
  void OnClick();

  void ButtonSetPosition(Vector3 pos) { SetPosition(pos); }

  void ButtonSetScale(Vector3 scale) {
    SetScale(scale);
    mAuxiliarScale.x = scale.x;
    mAuxiliarScale.y = scale.y;
    mAuxiliarScale.z = scale.z;
    SetHighlighted(mHighlighted);
  }

private:
  // Callback funtion
  std::function<void()> mOnClick;

  // Check if the button is highlighted
  bool mHighlighted;
  Vector3 mAuxiliarScale;
};