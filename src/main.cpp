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
