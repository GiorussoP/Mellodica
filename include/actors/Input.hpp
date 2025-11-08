#ifndef INPUT_HPP
#define INPUT_HPP

#include <SDL2/SDL_scancode.h>

class Input {
private:
  static Uint8 key_state[SDL_NUM_SCANCODES];
  static Uint8 last_key_state[SDL_NUM_SCANCODES];

public:
  static void Update();
  static bool WasKeyPressed(SDL_Scancode key);
  static bool IsKeyDown(SDL_Scancode key);
  static bool WasKeyReleased(SDL_Scancode key);
};

#endif