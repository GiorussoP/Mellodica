//
// Created by rapha on 28/11/2025.
//

#ifndef MELLODICA_UISCREEN_H
#define MELLODICA_UISCREEN_H

#include "../HUDElement.hpp"
#include "../UIButton.hpp"


class UIScreen {
public:
    enum class UIState {
        Active,
        Closing
    };

    UIScreen(class Game *game, const std::string& fontName);
    virtual ~UIScreen();

    virtual void Update(float deltaTime);
    virtual void HandleKeyPress(int key);

    void Close() {mState = UIState::Closing;}

    // HUDElements add
    HUDElement* AddImageOrElement(const std::string &hudTexturePath,
                                  const std::string &hudAtlasPath);

    HUDElement* AddImageOrElement(const std::string &singleImagePath);

    // UI Buttons add
    UIButton* AddButton(const std::string &hudTexturePath,
                        const std::string &hudAtlasPath,
                        std::function<void()> onClick);

    UIButton* AddButton(const std::string &singleImagePath,
                        std::function<void()> onClick);


protected:
    class Game* mGame;
    //class Font* mFont;

    UIState mState;

    int mSelectedElement;
    std::vector<HUDElement*> mHudImages;
    std::vector<UIButton*> mHudButtons;
};


#endif //MELLODICA_UISCREEN_H