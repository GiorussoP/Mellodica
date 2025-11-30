//
// Created by rapha on 28/11/2025.
//

#include "../../include/UI/UIButton.hpp"

UIButton::UIButton(Game *game, std::function<void()> onClick, const std::string &hudTexturePath,
                       const std::string &hudAtlasPath)
    : HUDElement(game, hudTexturePath, hudAtlasPath)
    , mOnClick(onClick)
    , mHighlighted(false)
    , mAuxiliarScale(1.0f)
{
}

UIButton::~UIButton() {
    //delete mAuxiliarScale;
}

UIButton::UIButton(Game *game, std::function<void()> onClick, const std::string &singleImagePath)
    : HUDElement(game, singleImagePath)
    , mOnClick(onClick)
    , mHighlighted(false)
    , mAuxiliarScale(1.0f)
{


}

void UIButton::OnClick() {
    if (mOnClick) {
        mOnClick();
    }
}