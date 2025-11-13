#include "actors/ShineActor.hpp"

#include "Game.hpp"
#include "Renderer.hpp"
#include "components/SpriteComponent.hpp"

ShineActor::ShineActor(Game *game, Vector3 color)
    : Actor(game), mLifetime(0.0f) {

  // Get atlas from renderer cache
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/textures/shine.json");
  // Get texture index from renderer cache
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/textures/shine.png");

  int textureIndex = game->GetRenderer()->GetTextureIndex(texture);

  mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);

  mSpriteComponent->SetColor(color);
  mSpriteComponent->AddAnimation(
      "shine", {"s1.png", "s2.png", "s3.png", "s4.png", "s5.png"}, false);

  mSpriteComponent->SetBloomed(true);
  mSpriteComponent->SetAnimation("shine");
  mSpriteComponent->SetAnimFPS(10.0f);
}

void ShineActor::OnUpdate(float deltaTime) {
  if (mLifetime <= 0.0f)
    return;
  mLifetime -= deltaTime;
  if (mLifetime <= 0.0f) {
    mSpriteComponent->SetVisible(false);
    mSpriteComponent->SetIsPaused(true);
  }
  mPosition += Vector3::UnitY * deltaTime * 0.5f;
  mScale -= Vector3::One * deltaTime * 0.5f;
}

void ShineActor::Start(float lifetime) {
  mLifetime = lifetime;
  mScale = Vector3::One;
  mSpriteComponent->SetVisible(true);
  mSpriteComponent->SetAnimationTimer(0.0f);
  mSpriteComponent->SetIsPaused(false);
}