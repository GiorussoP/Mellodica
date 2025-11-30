//
// Created by Lucas N. Ferreira on 22/05/25.
//

#pragma once

#include <functional>
#include <string>
#include "HUDElement.hpp"
#include "../Math.hpp"

class UIButton : HUDElement
{
public:
    UIButton(class Game* game, std::function<void()> onClick, const std::string &hudTexturePath,
             const std::string &hudAtlasPath);

    UIButton(class Game* game, std::function<void()> onClick, const std::string &singleImagePath);

    ~UIButton();

    void SetHighlighted(bool sel) {

        auto old = mHighlighted;
        mHighlighted = sel;

        if (old == false && mHighlighted == true) {
            mAuxiliarScale.x = mScale.x;
            mAuxiliarScale.y = mScale.y;
            mAuxiliarScale.z = mScale.z;
            mScale *= 2;
        }
        if (old == true && mHighlighted == false) {
            mScale.x = mAuxiliarScale.x;
            mScale.y = mAuxiliarScale.y;
            mScale.z = mAuxiliarScale.z;
        }
    }
    bool GetHighlighted() const { return mHighlighted; }
    /*
    void SetAuxiliarScale() {
        mAuxiliarScale = new Vector3(mScale.x, mScale.y, mScale.z);
    }*/

    // Called when button is clicked
    void OnClick();

    void ButtonSetPosition(Vector3 pos) {
        SetPosition(pos);
    }

    void ButtonSetScale(Vector3 scale) {
        SetScale(scale);
        mAuxiliarScale.x = scale.x;
        mAuxiliarScale.y = scale.y;
        mAuxiliarScale.z = scale.z;
    }

private:
    // Callback funtion
    std::function<void()> mOnClick;

    // Check if the button is highlighted
    bool mHighlighted;
    Vector3 mAuxiliarScale;
};