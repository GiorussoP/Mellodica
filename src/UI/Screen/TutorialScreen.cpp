//
// Created by luiza on 09/12/2025.
//

#include "../../../include/UI/Screen/TutorialScreen.hpp"
#include "../../../include/Game.hpp"
#include "../../../include/UI/TextElement.hpp"
#include <SDL2/SDL.h>

TutorialScreen::TutorialScreen(Game *game)
    : UIScreen(game, "fonts/MedodicaRegular.otf") {

  std::string instructions =
      "                   CONTROLES:   \n"
      " Setas      -  Movimentar personagem \n"
      " A, D       -  Movimentar câmera \n"
      " Espaço     -  Atirar nota \n"
      " (1,2,3,4,5,6,7,8,9,0,-,+) - Atirar nota específica \n\n"
      " Pressione [ENTER] para fechar ";

  auto textElement = AddText(instructions, Vector3(1.0f, 1.0f, 1.0f),
                             Vector3(0.0f, 0.0f, 0.0f), 0.75f);

  textElement->SetScale(Vector3(0.8f, 0.8f, 1.0f));
  textElement->SetPosition(Vector3(0.0f, 0.0f, 0.0f));
}

void TutorialScreen::HandleKeyPress(int key) {
  if (key == SDLK_RETURN || key == SDLK_RETURN2 || key == SDLK_KP_ENTER ||
      key == SDLK_ESCAPE) {
    Close();
  }
}