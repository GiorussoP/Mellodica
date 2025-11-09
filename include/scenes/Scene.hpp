#ifndef SCENE_HPP
#define SCENE_HPP

#include "Actor.hpp"
#include <string>
#include <unordered_set>
#include <vector>

class Game;

class Scene {

public:
  Scene(Game *game) : mGame(game) {}
  virtual ~Scene() {}

  virtual void Initialize() = 0;
  virtual void Cleanup();
  void RegisterActor(Actor *actor) { mActors.insert(actor); }
  void UnregisterActor(Actor *actor) { mActors.erase(actor); }

  void LoadLevel(const std::string &levelPath, unsigned int width = 1000,
                 unsigned int height = 1000);

private:
  std::unordered_set<Actor *> mActors;

protected:
  Game *mGame;
};

#endif