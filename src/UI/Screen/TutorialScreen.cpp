//
// Created by luiza on 09/12/2025.
//

#include "UI/Screen/TutorialScreen.hpp"
#include "Game.hpp"
#include "UI/TextElement.hpp"
#include <SDL2/SDL.h>

TutorialScreen::TutorialScreen(Game *game)
    : UIScreen(game, "fonts/MedodicaRegular.otf") {

  std::string instructions =
      "                   CONTROLS:   \n"
      " Arrow Keys -  Move character \n"
      " A, D       -  Move camera \n"
      " Space      -  Shoot note \n"
      " (1,2,3,4,5,6,7,8,9,0,-,+) - Shoot specific note \n\n"
      " Esc        -  Pause game \n"
      " F11        -  Toggle fullscreen \n\n"
      " Find an exit!\n\n"
      " Press [ENTER] to close ";

  auto textElement = AddText(instructions, Vector3(1.0f, 1.0f, 1.0f),
                             Vector3(0.0f, 0.0f, 0.0f), 0.75f);

  textElement->SetScale(Vector3(0.9f, 0.9f, 1.0f));
  textElement->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
}

void TutorialScreen::HandleKeyPress(int key) {
  if (key == SDLK_RETURN || key == SDLK_RETURN2 || key == SDLK_KP_ENTER ||
      key == SDLK_ESCAPE) {
    Close();
  }
}