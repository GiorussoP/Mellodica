#include "actors/TestActors.hpp"
#include "MIDI/MIDIPlayer.hpp"
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
      game->GetRenderer()->LoadTexture("./assets/textures/Blocks.png");
  // Get atlas from renderer cache
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/textures/Blocks.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  // Get shared mesh from renderer cache (only one instance created)
  Mesh *mesh = game->GetRenderer()->LoadMesh("cube");

  mMeshComponent =
      new MeshComponent(this, *mesh, texture, atlas, startingIndex);
  mMeshComponent->SetColor(color);
  // mColliderComponent = new
  // AABBCollider(this,ColliderLayer::Ground,Vector3::Zero,Vector3(0.5f,0.5f,0.5f));
}

GroundActor::GroundActor(Game *game, const Vector3 &color, int startingIndex)
    : Actor(game), mMeshComponent(nullptr) {
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/textures/Blocks.png");
  // Get atlas from renderer cache
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/textures/Blocks.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  // Get shared mesh from renderer cache (only one instance created)
  Mesh *mesh = game->GetRenderer()->LoadMesh("plane");

  mMeshComponent =
      new MeshComponent(this, *mesh, texture, atlas, startingIndex);
  mMeshComponent->SetColor(color);
}

void GroundActor::OnUpdate(float deltaTime) {
  // No specific behavior for ground - just render
}

MultiDrawablesActor::MultiDrawablesActor(Game *game)
    : Actor(game), mMeshComponent1(nullptr), mMeshComponent2(nullptr),
      mSpriteComponent(nullptr) {

  // First MeshComponent - Cube
  {
    Texture *texture =
        game->GetRenderer()->LoadTexture("./assets/textures/Blocks.png");
    TextureAtlas *atlas =
        game->GetRenderer()->LoadAtlas("./assets/textures/Blocks.json");
    atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));
    Mesh *mesh = game->GetRenderer()->LoadMesh("cube");
    mMeshComponent1 = new MeshComponent(this, *mesh, texture, atlas, 0);
    mMeshComponent1->SetColor(Color::Orange);
    mMeshComponent1->SetScale(Vector3(1.5f, 2.0f, 1.5f));
    mMeshComponent1->SetOffset(Vector3(0.0f, 0.5f, 0.0f));
  }

  // Second MeshComponent - Pyramid
  {
    Texture *texture =
        game->GetRenderer()->LoadTexture("./assets/textures/Blocks.png");
    TextureAtlas *atlas =
        game->GetRenderer()->LoadAtlas("./assets/textures/Blocks.json");
    atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));
    Mesh *mesh = game->GetRenderer()->LoadMesh("pyramid");
    mMeshComponent2 = new MeshComponent(this, *mesh, texture, atlas, 3);
    mMeshComponent2->SetColor(Color::Red);
    mMeshComponent2->SetRelativeRotation(
        Quaternion(Vector3::UnitY, Math::ToRadians(45.0f)));
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
  mRotation =
      Quaternion::Concatenate(mRotation, Quaternion(Vector3::UnitY, deltaTime));
  mPosition.y = 1.5f + 0.5f * Math::Sin(3.0f * SDL_GetTicks() / 1000.0f);

  mScale = Vector3::One *
           (0.75f + 0.25f * Math::Sin(4.0f * SDL_GetTicks() / 1000.0f));
}

PyramidActor::PyramidActor(Game *game, const Vector3 &color, int startingIndex)
    : Actor(game), mMeshComponent(nullptr) {

  // Get texture from renderer cache
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/textures/Blocks.png");

  // Get atlas from renderer cache
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/textures/Blocks.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));

  // Get shared mesh from renderer cache (only one instance created)
  Mesh *mesh = game->GetRenderer()->LoadMesh("pyramid");

  // Create mesh component with shared mesh and atlas
  mMeshComponent =
      new MeshComponent(this, *mesh, texture, atlas, startingIndex);
  mMeshComponent->SetColor(color);
  mMeshComponent->SetBloomed(true);
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
      mNotePlayerComponent(nullptr) {

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

  mNotePlayerComponent = new NotePlayerComponent(this, true, Vector3::Zero);
}

void GoombaActor::OnUpdate(float deltaTime) {
  auto notes = MIDIPlayer::pollNoteEvents();
  for (auto note : notes) {
    if (note.channel == 0 || note.channel == 2) {
      if (note.noteOn)
        // mPosition += Vector3::UnitY;
        mNotePlayerComponent->PlayNote(note.note, note.channel);
      else
        mNotePlayerComponent->EndNote(note.note);
    }
  }
}

// OBBTestActor implementation
OBBTestActor::OBBTestActor(Game *game)
    : Actor(game), mColliderComponent(nullptr) {
  // Register as always-active so it's included in collision detection
  game->AddAlwaysActive(this);

  // Create a tilted OBB collider with base size matching the cube mesh
  // The cube mesh is 1.0 unit, so half-extents are 0.5
  // The scale will be applied automatically to both mesh and collider
  mColliderComponent =
      new OBBCollider(this, ColliderLayer::Ground, Vector3::Zero,
                      Vector3(0.5f, 0.5f, 0.5f), true);

  // Get texture from renderer cache
  Texture *texture =
      game->GetRenderer()->LoadTexture("./assets/textures/Blocks.png");
  TextureAtlas *atlas =
      game->GetRenderer()->LoadAtlas("./assets/textures/Blocks.json");
  atlas->SetTextureIndex(game->GetRenderer()->GetTextureIndex(texture));
  Mesh *mesh = game->GetRenderer()->LoadMesh("cube");

  mMeshComponent = new MeshComponent(this, *mesh, texture, atlas, 0);
  SetScale(Vector3(7.0f, 1.0f, 1.0f));

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
