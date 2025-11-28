#include "Game.hpp"
#include "CSV.h"

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  // Game game;
  // bool success = game.Initialize();
  // if (success) {
  //   game.RunLoop();
  // }
  // game.Shutdown();

  MapReader mapReader("./assets/levels/levelTeste.csv");

  return 0;
}
