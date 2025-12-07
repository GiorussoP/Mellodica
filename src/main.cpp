#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include "CSV.h"
#include "Game.hpp"

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  Game game;
  bool success = game.Initialize();
  if (success) {
    game.RunLoop();
  }
  game.Shutdown();

  return 0;
}

