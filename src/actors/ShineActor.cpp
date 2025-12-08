#include "actors/ShineActor.hpp"

#include "AssetLoader.hpp"
#include "Game.hpp"
#include "render/Renderer.hpp"
#include "components/SpriteComponent.hpp"
#include <random>

ShineActor::ShineActor(Game *game, Vector3 color, bool autoDestroy)
    : Actor(game), mLifetime(0.0f), mAutoDestroy(autoDestroy) {
  game->AddAlwaysActive(this);
  // Get atlas from renderer cache
  TextureAtlas *atlas =
    game->GetRenderer()->LoadAtlas(getAssetPath("textures/shine.json"));
  // Get texture index from renderer cache
  Texture *texture =
      game->GetRenderer()->LoadTexture(getAssetPath("textures/shine.png"));

  int textureIndex = game->GetRenderer()->GetTextureIndex(texture);
  atlas->SetTextureIndex(textureIndex);

  mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);

  mSpriteComponent->SetColor(color);
  mSpriteComponent->AddAnimation(
      "shine", {"s1.png", "s2.png", "s3.png", "s4.png", "s5.png"}, false);

  mSpriteComponent->SetBloomed(true);
  mSpriteComponent->SetAnimation("shine");
  mSpriteComponent->SetAnimFPS(10.0f);

  // Start invisible and paused until Start() is called
  mSpriteComponent->SetVisible(false);
  mSpriteComponent->SetIsPaused(true);
}

void ShineActor::OnUpdate(float deltaTime) {
  // Skip update if not started yet (lifetime is still 0 and not visible)
  if (mLifetime <= 0.0f && !mSpriteComponent->IsVisible()) {
    return;
  }

  if (mLifetime <= 0.0f) {
    // Animation finished - destroy if marked or auto-destroy enabled
    if (mAutoDestroy || GetState() == ActorState::Destroy) {
      SetState(ActorState::Destroy);
    }
    return;
  }
  mLifetime -= deltaTime;
  if (mLifetime <= 0.0f) {
    mSpriteComponent->SetVisible(false);
    mSpriteComponent->SetIsPaused(true);
    if (mAutoDestroy) {
      SetState(ActorState::Destroy);
    }
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
