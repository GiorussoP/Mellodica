//
// Created by rapha on 29/11/2025.
//

#ifndef MELLODICA_MAINMENUSCREEN_H
#define MELLODICA_MAINMENUSCREEN_H

#endif //MELLODICA_MAINMENUSCREEN_H

#include "./UIScreen.hpp"

class MainMenuScreen : UIScreen {
    public:
    MainMenuScreen(class Game *game, const std::string& fontName);
    ~MainMenuScreen();

    void HandleKeyPress(int key) override;
    void Update(float deltaTime) override {};

};