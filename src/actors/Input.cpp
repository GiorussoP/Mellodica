//
// Created by grpas on 17/08/2025.
//
#include "Input.hpp"
#include <SDL2/SDL.h>
#include <cstring>
#include <initializer_list>

Uint8 Input::key_state[SDL_NUM_SCANCODES] = {0};
Uint8 Input::last_key_state[SDL_NUM_SCANCODES] = {0};

bool Input::WasKeyPressed(SDL_Scancode key) {
  return (bool)(key_state[key] && !last_key_state[key]);
}

void Input::Update() {
  std::memcpy(last_key_state, key_state, SDL_NUM_SCANCODES);
  const Uint8 *state = SDL_GetKeyboardState(nullptr);
  std::memcpy(key_state, state, SDL_NUM_SCANCODES);
}
bool Input::IsKeyDown(SDL_Scancode key) { return (bool)key_state[key]; }

bool Input::WasKeyReleased(SDL_Scancode key) {
  return (bool)(!key_state[key] && last_key_state[key]);
}