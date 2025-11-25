#ifndef TILE_ACTOR_HPP

#define TILE_ACTOR_HPP

#include "actors/Actor.hpp"
#include "components/ColliderComponent.hpp"
#include "components/MeshComponent.hpp"

enum class TileType { Ground, Fence, Wall, Limit };

class TileActor : public Actor {
public:
  TileActor(class Game *game, int gridX, int gridY, float tileSize);
  virtual ~TileActor() = default;
};

#endif // TILE_ACTOR_HPP