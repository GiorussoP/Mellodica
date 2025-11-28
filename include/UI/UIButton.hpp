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

    void SetHighlighted(bool sel) { mHighlighted = sel; }
    bool GetHighlighted() const { return mHighlighted; }

    // Called when button is clicked
    void OnClick();

private:
    // Callback funtion
    std::function<void()> mOnClick;

    // Check if the button is highlighted
    bool mHighlighted;
};