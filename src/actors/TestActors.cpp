#include "actors/TestActors.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include "MainMenu.hpp"
#include "TestScene.hpp"
#include "components/ColliderComponent.hpp"
#include "render/Mesh.hpp"
#include "render/Renderer.hpp"
#include "render/Texture.hpp"
#include "render/TextureAtlas.hpp"
#include <iostream>

// CubeActor implementation
CubeActor::CubeActor(Game *game, const Vector3 &color, int startingIndex)
    : Actor(game), mMeshComponent(nullptr) {
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/sprites/cubes.png");
  // Get atlas from renderer cache
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/sprites/cubes.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  // Get shared mesh from renderer cache (only one instance created)
  Mesh *mesh = game->GetRenderer()->LoadMesh("cube");

  mMeshComponent =
      new MeshComponent(this, *mesh, texture, atlas, startingIndex);
  mMeshComponent->SetColor(color);
  // mColliderComponent = new
  // AABBCollider(this,ColliderLayer::Ground,Vector3::Zero,Vector3(0.5f,0.5f,0.5f));
}

// SolidCubeActor implementation
SolidCubeActor::SolidCubeActor(Game *game, const Vector3 &color,
                               int startingIndex)
    : CubeActor(game, color, startingIndex), mColliderComponent(nullptr) {
  // Add AABB collider
  mColliderComponent = new AABBCollider(
      this, ColliderLayer::Ground, Vector3::Zero, Vector3(0.5f, 0.5f, 0.5f));
}

void SolidCubeActor::OnUpdate(float deltaTime) {
  // Call base class update
  CubeActor::OnUpdate(deltaTime);
}

// WallActor implementation
WallActor::WallActor(Game *game, const Vector3 &color, int startingIndex)
    : Actor(game), mMeshComponent(nullptr) {
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/sprites/wall.png");
  // Get atlas from renderer cache
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/sprites/wall.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  // Get shared mesh from renderer cache (only one instance created)
  Mesh *mesh = game->GetRenderer()->LoadMesh("wall");

  mMeshComponent =
      new MeshComponent(this, *mesh, texture, atlas, startingIndex);
  mMeshComponent->SetColor(color);
}

void WallActor::OnUpdate(float deltaTime) {
  // No specific behavior for wall - just render
}

// SolidWallActor implementation
SolidWallActor::SolidWallActor(Game *game, const Vector3 &color,
                               int startingIndex)
    : WallActor(game, color, startingIndex), mColliderComponent(nullptr) {
  // Add AABB collider (wall is 1x3x1 units, so half-extents 0.5, 1.5, 0.5)
  mColliderComponent = new AABBCollider(
      this, ColliderLayer::Ground, Vector3::Zero, Vector3(0.5f, 1.5f, 0.5f));
}

void SolidWallActor::OnUpdate(float deltaTime) {
  // Call base class update
  WallActor::OnUpdate(deltaTime);
}

DoorWall::DoorWall(Game *game, const Vector3 &color)
    : SolidWallActor(game, color, 16), mRoofComponent(nullptr) {
  // Add roof MeshComponent - Pyramid
  {
    Texture *texture =
        game->GetRenderer()->LoadTexture("./assets/sprites/floor.png");
    TextureAtlas *atlas =
        game->GetRenderer()->LoadAtlas("./assets/sprites/floor.json");
    atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));
    Mesh *mesh = game->GetRenderer()->LoadMesh("pyramid");
    mRoofComponent = new MeshComponent(this, *mesh, texture, atlas, 50);
    mRoofComponent->SetColor(Color::White);
    mRoofComponent->SetOffset(Vector3(0.0f, 1.75f, 0.0f));
    mRoofComponent->SetScale(Vector3(1.5f, 0.5f, 1.5f));
  }
}

void DoorWall::OnUpdate(float deltaTime) {
  SolidWallActor::OnUpdate(deltaTime);
}

WindowWall::WindowWall(Game *game, const Vector3 &color)
    : SolidWallActor(game, color, 12), mRoofComponent(nullptr) {
  // Add roof MeshComponent - Pyramid
  {
    Texture *texture =
        game->GetRenderer()->LoadTexture("./assets/sprites/floor.png");
    TextureAtlas *atlas =
        game->GetRenderer()->LoadAtlas("./assets/sprites/floor.json");
    atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));
    Mesh *mesh = game->GetRenderer()->LoadMesh("pyramid");
    mRoofComponent = new MeshComponent(this, *mesh, texture, atlas, 50);
    mRoofComponent->SetColor(Color::White);
    mRoofComponent->SetOffset(Vector3(0.0f, 1.75f, 0.0f));
    mRoofComponent->SetScale(Vector3(1.5f, 0.5f, 1.5f));
  }
}

void WindowWall::OnUpdate(float deltaTime) {
  SolidWallActor::OnUpdate(deltaTime);
}

TreeActor::TreeActor(Game *game) : Actor(game) {
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/sprites/tree.png");
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/sprites/tree.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  mSpriteComponent = new SpriteComponent(
      this, game->GetRenderer()->GetTextureIndex(texture), atlas);
  mSpriteComponent->SetColor(Color::White);
  mSpriteComponent->SetOffset(Vector3(0.0f, 0.25f, 0.0f));
  mSpriteComponent->SetScale(Vector3(1.5f, 1.5f, 1.0f));
  mSpriteComponent->AddAnimation("idle", {"tree-64x64.png"});
  mSpriteComponent->SetAnimFPS(0.0f);
  mSpriteComponent->SetAnimation("idle");

  mColliderComponent = new SphereCollider(
      this, ColliderLayer::Ground, Vector3(0.0f, 0.0f, 0.0f), 0.5f, true);
}

VisualTree::VisualTree(Game *game) : Actor(game) {
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/sprites/tree.png");
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/sprites/tree.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  mSpriteComponent = new SpriteComponent(
      this, game->GetRenderer()->GetTextureIndex(texture), atlas);
  mSpriteComponent->SetColor(Color::White);
  mSpriteComponent->SetOffset(Vector3(0.0f, 0.25f, 0.0f));
  mSpriteComponent->SetScale(Vector3(1.5f, 1.5f, 1.0f));
  mSpriteComponent->AddAnimation("idle", {"tree-64x64.png"});
  mSpriteComponent->SetAnimFPS(0.0f);
  mSpriteComponent->SetAnimation("idle");
}

SmallRockActor::SmallRockActor(Game *game) : Actor(game) {
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/sprites/medium_nature.png");
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/sprites/medium_nature.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  mSpriteComponent = new SpriteComponent(
      this, game->GetRenderer()->GetTextureIndex(texture), atlas);
  mSpriteComponent->SetColor(Color::White);
  mSpriteComponent->SetOffset(Vector3(0.0f, 0.0f, 0.0f));
  mSpriteComponent->SetScale(Vector3(1.0f, 1.0f, 1.0f));
  mSpriteComponent->AddAnimation("idle", {"little-rock-32x32.png"});
  mSpriteComponent->SetAnimFPS(0.0f);
  mSpriteComponent->SetAnimation("idle");

  mColliderComponent = new SphereCollider(
      this, ColliderLayer::Ground, Vector3(0.0f, 0.0f, 0.0f), 0.125f, true);
}

MediumRockActor::MediumRockActor(Game *game) : Actor(game) {
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/sprites/medium_nature.png");
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/sprites/medium_nature.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  mSpriteComponent = new SpriteComponent(
      this, game->GetRenderer()->GetTextureIndex(texture), atlas);
  mSpriteComponent->SetColor(Color::White);
  mSpriteComponent->SetOffset(Vector3(0.0f, 0.0f, 0.0f));
  mSpriteComponent->SetScale(Vector3(1.0f, 1.0f, 1.0f));
  mSpriteComponent->AddAnimation("idle", {"medium-rock-32x32.png"});
  mSpriteComponent->SetAnimFPS(0.0f);
  mSpriteComponent->SetAnimation("idle");

  mColliderComponent = new SphereCollider(
      this, ColliderLayer::Ground, Vector3(0.0f, 0.0f, 0.0f), 0.25f, true);
}

BushActor::BushActor(Game *game) : Actor(game) {
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/sprites/medium_nature.png");
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/sprites/medium_nature.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  mSpriteComponent = new SpriteComponent(
      this, game->GetRenderer()->GetTextureIndex(texture), atlas);
  mSpriteComponent->SetColor(Color::White);
  mSpriteComponent->SetOffset(Vector3(0.0f, 0.0f, 0.0f));
  mSpriteComponent->SetScale(Vector3(1.0f, 1.0f, 1.0f));
  mSpriteComponent->AddAnimation("idle", {"bush-32x32.png"});
  mSpriteComponent->SetAnimFPS(0.0f);
  mSpriteComponent->SetAnimation("idle");

  mColliderComponent = new SphereCollider(
      this, ColliderLayer::Ground, Vector3(0.0f, 0.0f, 0.0f), 0.5f, true);
}

GrassActorA::GrassActorA(Game *game) : Actor(game), mSpriteComponent(nullptr) {
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/sprites/grass.png");
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/sprites/grass.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  mSpriteComponent = new SpriteComponent(
      this, game->GetRenderer()->GetTextureIndex(texture), atlas);
  mSpriteComponent->SetColor(Color::White);
  mSpriteComponent->SetOffset(Vector3(0.0f, -0.25f, 0.0f));
  mSpriteComponent->SetScale(Vector3(0.5f, 0.5f, 1.0f));
  mSpriteComponent->AddAnimation("idle", {"grass1-16x16.png"});
  mSpriteComponent->SetAnimFPS(0.0f);
  mSpriteComponent->SetAnimation("idle");
}

GrassActorB::GrassActorB(Game *game) : Actor(game), mSpriteComponent(nullptr) {
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/sprites/grass.png");
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/sprites/grass.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  mSpriteComponent = new SpriteComponent(
      this, game->GetRenderer()->GetTextureIndex(texture), atlas);
  mSpriteComponent->SetColor(Color::White);
  mSpriteComponent->SetOffset(Vector3(0.0f, -0.25f, 0.0f));
  mSpriteComponent->SetScale(Vector3(0.5f, 0.5f, 1.0f));
  mSpriteComponent->AddAnimation("idle", {"grass2-16x16.png"});
  mSpriteComponent->SetAnimFPS(0.0f);
  mSpriteComponent->SetAnimation("idle");
}

GrassActorC::GrassActorC(Game *game) : Actor(game), mSpriteComponent(nullptr) {
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/sprites/grass.png");
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/sprites/grass.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  mSpriteComponent = new SpriteComponent(
      this, game->GetRenderer()->GetTextureIndex(texture), atlas);
  mSpriteComponent->SetColor(Color::White);
  mSpriteComponent->SetOffset(Vector3(0.0f, -0.25f, 0.0f));
  mSpriteComponent->SetScale(Vector3(0.5f, 0.5f, 1.0f));
  mSpriteComponent->AddAnimation("idle", {"grass3-16x16.png"});
  mSpriteComponent->SetAnimFPS(0.0f);
  mSpriteComponent->SetAnimation("idle");
}

GroundActor::GroundActor(Game *game, const Vector3 &color, int startingIndex)
    : Actor(game), mMeshComponent(nullptr) {
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/sprites/floor.png");
  // Get atlas from renderer cache
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/sprites/floor.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  // Get shared mesh from renderer cache (only one instance created)
  Mesh *mesh = game->GetRenderer()->LoadMesh("plane");

  mMeshComponent =
      new MeshComponent(this, *mesh, texture, atlas, startingIndex);
  mMeshComponent->SetColor(color);
}

void GroundActor::OnUpdate(float deltaTime) {
  // No specific behavior for ground - just rendere
}

MultiDrawablesActor::MultiDrawablesActor(Game *game)
    : Actor(game), mMeshComponent1(nullptr), mMeshComponent2(nullptr),
      mSpriteComponent(nullptr) {

  // First MeshComponent - Cube
  {
    Texture *texture =
        game->GetRenderer()->LoadTexture("./assets/sprites/wall.png");
    TextureAtlas *atlas =
        game->GetRenderer()->LoadAtlas("./assets/sprites/wall.json");
    atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));
    Mesh *mesh = game->GetRenderer()->LoadMesh("cube");
    mMeshComponent1 = new MeshComponent(this, *mesh, texture, atlas, 5);
    mMeshComponent1->SetColor(Color::White);
    mMeshComponent1->SetScale(Vector3(1.5f, 2.0f, 1.5f));
    mMeshComponent1->SetOffset(Vector3(0.0f, 0.5f, 0.0f));
  }

  mColliderComponent =
      new OBBCollider(this, ColliderLayer::Ground, Vector3::Zero,
                      Vector3(0.75f, 1.0f, 0.75f), true);

  // Second MeshComponent - Pyramid
  {
    Texture *texture =
        game->GetRenderer()->LoadTexture("./assets/sprites/floor.png");
    TextureAtlas *atlas =
        game->GetRenderer()->LoadAtlas("./assets/sprites/floor.json");
    atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));
    Mesh *mesh = game->GetRenderer()->LoadMesh("pyramid");
    mMeshComponent2 = new MeshComponent(this, *mesh, texture, atlas, 50);
    mMeshComponent2->SetColor(Color::White);
    mMeshComponent2->SetOffset(Vector3(0.0f, 1.75f, 0.0f));
    mMeshComponent2->SetScale(Vector3(2.0f, 0.5f, 2.0f));
  }

  // SpriteComponent
  {
    TextureAtlas *atlas =
        game->GetRenderer()->LoadAtlas("./assets/textures/shine.json");
    Texture *texture =
        game->GetRenderer()->LoadTexture("./assets/textures/shine.png");
    int textureIndex = game->GetRenderer()->GetTextureIndex(texture);
    atlas->SetTextureIndex(textureIndex);
    mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);
    mSpriteComponent->SetColor(Color::Blue);
    mSpriteComponent->AddAnimation("sloop",
                                   {"s1.png", "s2.png", "s3.png", "s4.png",
                                    "s5.png", "s4.png", "s3.png", "s2.png"},
                                   true);
    mSpriteComponent->SetAnimation("sloop");
    mSpriteComponent->SetAnimFPS(12.0f);
    mSpriteComponent->SetOffset(Vector3(0.0f, 3.0f, 0.0f));
    mSpriteComponent->SetScale(Vector3(1.0f, 1.0f, 1.0f));
  }
}

void MultiDrawablesActor::OnUpdate(float deltaTime) {
  mRotation = Quaternion::Concatenate(
      mRotation, Quaternion(Vector3::UnitY, deltaTime * 0.05));

  mPosition.y = 2.0f - 1.0f * Math::Sin(4.0f * SDL_GetTicks() / 10000.0f);

  mScale = Vector3(5.0f + 3.0f * Math::Cos(4.0f * SDL_GetTicks() / 10000.0f),
                   3.0f - 2.0f * Math::Sin(4.0f * SDL_GetTicks() / 10000.0f),
                   (5.0f - 3.0f * Math::Sin(4.0f * SDL_GetTicks() / 10000.0f)));
}

PyramidActor::PyramidActor(Game *game, const Vector3 &color, int startingIndex)
    : Actor(game), mMeshComponent(nullptr) {

  // Get texture from renderer cache
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/sprites/cubes.png");

  // Get atlas from renderer cache
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/sprites/cubes.json");

  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  // Get shared mesh from renderer cache (only one instance created)
  Mesh *mesh = game->GetRenderer()->LoadMesh("pyramid");

  // Create mesh component with shared mesh and atlas
  mMeshComponent =
      new MeshComponent(this, *mesh, texture, atlas, startingIndex);
  mMeshComponent->SetColor(color);
  mMeshComponent->SetBloomed(true);
}

HouseActor::HouseActor(Game *game)
    : Actor(game), mMeshComponent1(nullptr), mMeshComponent2(nullptr) {

  // First MeshComponent - Cube
  {
    Texture *texture =
        game->GetRenderer()->LoadTexture("./assets/sprites/cubes.png");
    TextureAtlas *atlas =
        game->GetRenderer()->LoadAtlas("./assets/sprites/cubes.json");
    atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));
    Mesh *mesh = game->GetRenderer()->LoadMesh("cube");
    mMeshComponent1 = new MeshComponent(this, *mesh, texture, atlas, 5);
    mMeshComponent1->SetColor(Color::White);
    mMeshComponent1->SetScale(Vector3(1.5f, 2.0f, 1.5f));
    mMeshComponent1->SetOffset(Vector3(0.0f, 0.5f, 0.0f));
  }

  mColliderComponent =
      new OBBCollider(this, ColliderLayer::Ground, Vector3(0.0f, 0.5f, 0.0f),
                      Vector3(0.75f, 1.0f, 0.75f), true);

  // Second MeshComponent - Pyramid
  {
    Texture *texture =
        game->GetRenderer()->LoadTexture("./assets/sprites/floor.png");
    TextureAtlas *atlas =
        game->GetRenderer()->LoadAtlas("./assets/sprites/floor.json");
    atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));
    Mesh *mesh = game->GetRenderer()->LoadMesh("pyramid");
    mMeshComponent2 = new MeshComponent(this, *mesh, texture, atlas, 50);
    mMeshComponent2->SetColor(Color::White);
    mMeshComponent2->SetOffset(Vector3(0.0f, 1.75f, 0.0f));
    mMeshComponent2->SetScale(Vector3(2.0f, 0.5f, 2.0f));
  }
}

void PyramidActor::OnUpdate(float deltaTime) {}

void CubeActor::OnUpdate(float deltaTime) {}

// MarioActor implementation
MarioActor::MarioActor(Game *game) : Actor(game), mSpriteComponent(nullptr) {
  // Get atlas from renderer cache
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/textures/Mario.json");

  // Get texture index from renderer cache
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/textures/Mario.png");
  int textureIndex = game->GetRenderer()->GetTextureIndex(texture);
  atlas->SetTextureIndex(textureIndex);

  // Create sprite component with atlas
  mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);

  // Setup running animation using atlas tile indices for Run1, Run2, Run3
  mSpriteComponent->AddAnimation("run", {"Run1.png", "Run2.png", "Run3.png"});
  mSpriteComponent->SetAnimation("run");
  mSpriteComponent->SetAnimFPS(8.0f);

  // Collider component - static since Mario doesn't move
  mColliderComponent = new SphereCollider(this, ColliderLayer::Player,
                                          Vector3::Zero, 0.5f, true);
}

void MarioActor::OnUpdate(float deltaTime) {}

GoombaActor::GoombaActor(Game *game)
    : Actor(game), mSpriteComponent(nullptr), mColliderComponent(nullptr),
      mNotePlayerActor(nullptr) {

  mGame->AddAlwaysActive(this);
  // Get atlas from renderer cache
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/textures/Goomba.json");

  // Get texture index from renderer cache
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/textures/Goomba.png");
  int textureIndex = game->GetRenderer()->GetTextureIndex(texture);
  atlas->SetTextureIndex(textureIndex);

  // Create sprite component with atlas
  mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);

  // Setup running animation with Walk0 and Walk1 frames
  mSpriteComponent->AddAnimation("run", {"Walk0.png", "Walk1.png"});
  mSpriteComponent->SetAnimation("run");
  mSpriteComponent->SetAnimFPS(2.0f);

  mColliderComponent = new AABBCollider(
      this, ColliderLayer::Player, Vector3::Zero, Vector3(0.5f, 0.5f, 0.5f));

  mNotePlayerActor = new NotePlayerActor(mGame, true);
}

void GoombaActor::OnUpdate(float deltaTime) {}

// OBBTestActor implementation
OBBTestActor::OBBTestActor(Game *game)
    : Actor(game), mColliderComponent(nullptr) {
  // Register as always-active so it's included in collision detection
  game->AddAlwaysActive(this);

  // Create a tilted OBB collider with base size matching the wall mesh
  // The wall mesh is 1.0 unit in X/Z and 3.0 units in Y, so half-extents are
  // 0.5, 1.5, 0.5 The scale will be applied automatically to both mesh and
  // collider
  mColliderComponent =
      new OBBCollider(this, ColliderLayer::Ground, Vector3::Zero,
                      Vector3(0.5f, 0.5f, 0.5f), true);

  // Get texture from renderer cache
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/sprites/cubes.png");
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/sprites/cubes.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));
  Mesh *mesh = game->GetRenderer()->LoadMesh("cube");

  mMeshComponent = new MeshComponent(this, *mesh, texture, atlas, 4);
  SetScale(Vector3(1.0f, 1.0f, 1.0f));

  // Rotate 45 degrees around Y axis
  Quaternion rotation = Quaternion(Vector3::UnitY, Math::ToRadians(45.0f));
  SetRotation(rotation);
}

void OBBTestActor::OnUpdate(float deltaTime) {
  // Static actor - no update needed
}

MIDIControlActor::MIDIControlActor(Game *game)
    : Actor(game), mPrevSpacePressed(false), mPrevRPressed(false),
      mPrevJPressed(false), mPrevMPressed(false), mPrevUPressed(false),
      mPrevPlusPressed(false), mPrevMinusPressed(false), mPrevPPressed(false) {
  // Register as always-active so controls work anywhere
  game->AddAlwaysActive(this);

  std::cout << "\n=== MIDI Control Actor Created ===" << std::endl;
  std::cout << "Controls:" << std::endl;
  std::cout << "  SPACE    - Play/Pause" << std::endl;
  std::cout << "  R        - Reload song" << std::endl;
  std::cout << "  J        - Jump to 30 seconds" << std::endl;
  std::cout << "  M        - Mute channel 0" << std::endl;
  std::cout << "  U        - Unmute channel 0" << std::endl;
  std::cout << "  +/=      - Increase speed (1.5x)" << std::endl;
  std::cout << "  -        - Decrease speed (0.5x)" << std::endl;
  std::cout << "  P        - Print current channel states" << std::endl;
  std::cout << "===================================\n" << std::endl;
}

void MIDIControlActor::OnProcessInput() {
  // SPACE - Play/Pause toggle
  bool spacePressed = Input::WasKeyPressed(SDL_SCANCODE_SPACE);
  if (spacePressed && !mPrevSpacePressed) {
    // Thread-safe: can call from UpdateActors without issues
    static bool isPlaying = true;
    if (isPlaying) {
      std::cout << "[MIDI] Pausing..." << std::endl;
      MIDIPlayer::pause();
      isPlaying = false;
    } else {
      std::cout << "[MIDI] Playing..." << std::endl;
      MIDIPlayer::play();
      isPlaying = true;
    }
  }
  mPrevSpacePressed = spacePressed;

  // R - Reload song
  bool rPressed = Input::WasKeyPressed(SDL_SCANCODE_R);
  if (rPressed && !mPrevRPressed) {
    std::cout << "[MIDI] Reloading song..." << std::endl;
    // Thread-safe: loadSong acquires mutex
    MIDIPlayer::jumpTo(0.0f); // Reset position
  }
  mPrevRPressed = rPressed;

  // J - Jump to 30 seconds
  bool jPressed = Input::WasKeyPressed(SDL_SCANCODE_J);
  if (jPressed && !mPrevJPressed) {
    std::cout << "[MIDI] Jumping to 30 seconds..." << std::endl;
    // Thread-safe: jumpTo acquires mutex and resets positions
    MIDIPlayer::jumpTo(30.0f);
  }
  mPrevJPressed = jPressed;

  // M - Mute channel 0
  bool mPressed = Input::WasKeyPressed(SDL_SCANCODE_M);
  if (mPressed && !mPrevMPressed) {
    std::cout << "[MIDI] Muting channel 0..." << std::endl;
    // Thread-safe: muteChannel acquires mutex
    MIDIPlayer::muteChannel(0);
  }
  mPrevMPressed = mPressed;

  // U - Unmute channel 0
  bool uPressed = Input::WasKeyPressed(SDL_SCANCODE_U);
  if (uPressed && !mPrevUPressed) {
    std::cout << "[MIDI] Unmuting channel 0..." << std::endl;
    // Thread-safe: unmuteChannel acquires mutex
    MIDIPlayer::unmuteChannel(0);
  }
  mPrevUPressed = uPressed;

  // + (or =) - Increase speed
  bool plusPressed = Input::WasKeyPressed(SDL_SCANCODE_EQUALS) ||
                     Input::WasKeyPressed(SDL_SCANCODE_KP_PLUS);
  if (plusPressed && !mPrevPlusPressed) {
    std::cout << "[MIDI] Increasing speed to 1.5x..." << std::endl;
    // Thread-safe: setSpeed acquires mutex
    MIDIPlayer::setSpeed(1.5);
  }
  mPrevPlusPressed = plusPressed;

  // - Decrease speed
  bool minusPressed = Input::WasKeyPressed(SDL_SCANCODE_MINUS) ||
                      Input::WasKeyPressed(SDL_SCANCODE_KP_MINUS);
  if (minusPressed && !mPrevMinusPressed) {
    std::cout << "[MIDI] Decreasing speed to 0.5x..." << std::endl;
    // Thread-safe: setSpeed acquires mutex
    MIDIPlayer::setSpeed(0.5);
  }
  mPrevMinusPressed = minusPressed;

  // P - Print current channel states
  bool pPressed = Input::WasKeyPressed(SDL_SCANCODE_P);
  if (pPressed && !mPrevPPressed) {
    std::cout << "\n=== Current MIDI Channel States ===" << std::endl;
    // Thread-safe: getChannels returns reference but mutex protects access
    auto &channels = MIDIPlayer::getChannels();

    for (int i = 0; i < channels.size(); ++i) {
      const auto &channel = channels[i];

      if (!channel.active) {
        std::cout << "Channel " << i << ": INACTIVE" << std::endl;
        continue;
      }

      std::cout << "Channel " << i << ": ACTIVE" << std::endl;
      std::cout << "  Total notes: " << channel.notes.size() << std::endl;
      std::cout << "  Current position: " << channel.pos << "/"
                << channel.notes.size() << std::endl;

      // Show next few upcoming notes
      if (channel.pos < channel.notes.size()) {
        std::cout << "  Next notes:" << std::endl;
        int notesToShow =
            std::min(5, static_cast<int>(channel.notes.size() - channel.pos));
        for (int j = 0; j < notesToShow; ++j) {
          const auto &note = channel.notes[channel.pos + j];
          std::cout << "    [" << (channel.pos + j) << "] "
                    << "Time: " << note.start << "s, "
                    << "Note: " << note.note << ", " << (note.on ? "ON" : "OFF")
                    << ", "
                    << "Vel: " << note.velocity << std::endl;
        }
      } else {
        std::cout << "  (All notes played)" << std::endl;
      }
    }
    std::cout << "===================================\n" << std::endl;
  }
  mPrevPPressed = pPressed;
}

TriggerActor::TriggerActor(Game *game) : Actor(game), mTriggered(false) {
  // Register as always-active so it's included in collision detection
  game->AddAlwaysActive(this);

  // Add AABB collider
  new AABBCollider(this, ColliderLayer::Entity, Vector3::Zero,
                   Vector3(1.0f, 1.0f, 1.0f), true);
}

void TriggerActor::OnCollision(Vector3 penetration, ColliderComponent *other) {
  if (!mTriggered && other->GetOwner() == mGame->GetPlayer()) {
    mGame->LoadScene(new MainMenu(mGame));
    mTriggered = true;
  }
}