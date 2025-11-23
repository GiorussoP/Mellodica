#ifndef MAIN_MENU_HPP
#define MAIN_MENU_HPP

#include "Scene.hpp"

class MainMenu : public Scene {

public:
  MainMenu(Game *game) : Scene(game) {};
  void Initialize() override;
};

#endif