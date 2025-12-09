//
// Created by luiza on 09/12/2025.
//

#ifndef PAUSESCREEN_HPP
#define PAUSESCREEN_HPP

#include "UIScreen.hpp"

class PauseScreen : public UIScreen {
public:
    PauseScreen(class Game* game);
    ~PauseScreen();

    void HandleKeyPress(int key) override;

private:
    class HUDElement* mBackground;
};

#endif //PAUSESCREEN_HPP