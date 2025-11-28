//
// Created by rapha on 28/11/2025.
//

#include "../../../include/UI/Screen/UIScreen.hpp"
#include "../../../include/Game.hpp"
#include "../../../include/UI/HUDElement.hpp"
#include "../../../include/UI/UIButton.hpp"

UIScreen::UIScreen(class Game *game, const std::string &fontName)
    :mGame(game)
    ,mState(UIState::Active)
    ,mSelectedElement(-1)
{
    mGame->PushUI(this);
    SDL_Log("UIScreen::UIScreen - Added UI to the Game UI Stack");
}

UIScreen::~UIScreen() {
    for (auto img: mHudImages) {
        delete img;
    }
    mHudImages.clear();
    for (auto button : mHudButtons) {
        delete button;
    }
    mHudButtons.clear();
}

HUDElement* UIScreen::AddImageOrElement(const std::string &hudTexturePath, const std::string &hudAtlasPath) {
    auto hE = new HUDElement(mGame, hudTexturePath, hudAtlasPath);
    mHudImages.push_back(hE);
    return hE;
}

HUDElement* UIScreen::AddImageOrElement(const std::string &singleImagePath) {
    auto hE = new HUDElement(mGame, singleImagePath);
    mHudImages.push_back(hE);
    return hE;
}

UIButton *UIScreen::AddButton(const std::string &hudTexturePath, const std::string &hudAtlasPath, std::function<void()> onClick) {
    auto hB = new UIButton(mGame, onClick, hudTexturePath, hudAtlasPath);
    mHudButtons.push_back(hB);
    return hB;
}

UIButton *UIScreen::AddButton(const std::string &singleImagePath, std::function<void()> onClick) {
    auto hB = new UIButton(mGame, onClick, singleImagePath);
    mHudButtons.push_back(hB);
    return hB;
}

void UIScreen::HandleKeyPress(int key)
{
    // Navigate between buttons with UP/DOWN arrows
    if (key == SDLK_UP || key == SDLK_w)
    {
        // Move to previous button
        if (mSelectedElement > 0)
        {
            mHudButtons[mSelectedElement]->SetHighlighted(false);
            mSelectedElement--;
            mHudButtons[mSelectedElement]->SetHighlighted(true);
        }
    }
    else if (key == SDLK_DOWN || key == SDLK_s)
    {
        // Move to next button
        if (mSelectedElement < static_cast<int>(mHudButtons.size()) - 1)
        {
            mHudButtons[mSelectedElement]->SetHighlighted(false);
            mSelectedElement++;
            mHudButtons[mSelectedElement]->SetHighlighted(true);
        }
    }
    else if (key == SDLK_RETURN)
    {
        // Click the currently selected button
        if (mSelectedElement >= 0 && mSelectedElement < static_cast<int>(mHudButtons.size()))
        {
            mHudButtons[mSelectedElement]->OnClick();
        }
    }
}
