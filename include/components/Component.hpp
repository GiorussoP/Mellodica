#pragma once
#include "Input.hpp"
#include <SDL2/SDL.h>

class Component {
public:
  // Constructor (lower update order = earlier update)
  Component(class Actor *owner, int updateOrder = 100);
  virtual ~Component();

  // Update this component by delta time
  virtual void Update(float deltaTime);

  // Process input for this component
  virtual void ProcessInput();

  // For debugging: draw component-specific info
  virtual void DebugDraw(class Renderer *renderer) {};

  int GetUpdateOrder() const { return mUpdateOrder; }
  class Actor *GetOwner() const { return mOwner; }
  class Game *GetGame() const;

  void SetEnabled(bool enabled) { mIsEnabled = enabled; }
  bool IsEnabled() const { return mIsEnabled; }

protected:
  class Actor *mOwner;
  int mUpdateOrder;
  bool mIsEnabled;
};
