//
// Created by luiza on 09/12/2025.
//

#ifndef TUTORIALSCREEN_HPP
#define TUTORIALSCREEN_HPP

#include "UIScreen.hpp"

class TutorialScreen : public UIScreen {
public:
    TutorialScreen(class Game* game);

    void HandleKeyPress(int key) override;
};

#endif //TUTORIALSCREEN_HPP