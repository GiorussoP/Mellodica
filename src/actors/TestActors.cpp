#include "actors/TestActors.hpp"
#include "render/TextureAtlas.hpp"
#include "render/Mesh.hpp"
#include "render/Renderer.hpp"
#include "render/Texture.hpp"
#include "MIDI/MIDIPlayer.hpp"
#include <iostream>


// CameraController implementation
CameraController::CameraController(Game* game)
: Actor(game)
, mMoveForward(false)
, mMoveBackward(false)
, mMoveLeft(false)
, mMoveRight(false)
, mRotateLeft(false)
, mRotateRight(false)
, mRotateUp(false)
, mRotateDown(false)
{
    // Register as always-active so camera works even when far from origin
    game->AddAlwaysActive(this);

    std::cout<<"\n============================================="<<std::endl;
    std::cout << "CameraController created - WASD to move and ARROWS to look" << std::endl;
    std::cout<<"============================================="<<std::endl;
}

void CameraController::OnProcessInput(const Uint8* keyState)
{
    // Just store the input state
    mMoveForward = keyState[SDL_SCANCODE_W];
    mMoveBackward = keyState[SDL_SCANCODE_S];
    mMoveLeft = keyState[SDL_SCANCODE_A];
    mMoveRight = keyState[SDL_SCANCODE_D];
    mRotateLeft = keyState[SDL_SCANCODE_LEFT];
    mRotateRight = keyState[SDL_SCANCODE_RIGHT];
    mRotateUp = keyState[SDL_SCANCODE_UP];
    mRotateDown = keyState[SDL_SCANCODE_DOWN];
}

void CameraController::OnUpdate(float deltaTime)
{
    Game* game = GetGame();
    Vector3 cameraPos = game->GetCameraPos();
    Vector3 cameraForward = game->GetCameraForward();
    Vector3 cameraUp = game->GetCameraUp();
    float yaw = game->GetCameraYaw();
    float pitch = game->GetCameraPitch();
    

    // Movement (WASD)
    if (mMoveForward)
    {
        cameraPos += cameraForward * CAMERA_MOVE_SPEED * deltaTime;
    }
    if (mMoveBackward)
    {
        cameraPos -= cameraForward * CAMERA_MOVE_SPEED * deltaTime;
    }
    if (mMoveLeft)
    {
        Vector3 right = Vector3::Cross(cameraForward, cameraUp);
        right.Normalize();
        cameraPos += right * CAMERA_MOVE_SPEED * deltaTime;
    }
    if (mMoveRight)
    {
        Vector3 right = Vector3::Cross(cameraForward, cameraUp);
        right.Normalize();
        cameraPos -= right * CAMERA_MOVE_SPEED * deltaTime;
    }
    
    // Rotation (Arrow keys)
    if (mRotateLeft)
    {
        yaw += CAMERA_ROTATE_SPEED * deltaTime;
    }
    if (mRotateRight)
    {
        yaw -= CAMERA_ROTATE_SPEED * deltaTime;
    }
    if (mRotateUp)
    {
        pitch += CAMERA_ROTATE_SPEED * deltaTime;
        if (pitch > 89.0f) pitch = 89.0f;
    }
    if (mRotateDown)
    {
        pitch -= CAMERA_ROTATE_SPEED * deltaTime;
        if (pitch < -89.0f) pitch = -89.0f;
    }
    
    // Update camera direction from yaw and pitch
    float yawRad = Math::ToRadians(yaw);
    float pitchRad = Math::ToRadians(pitch);
    
    cameraForward.x = Math::Sin(yawRad) * Math::Cos(pitchRad);
    cameraForward.y = Math::Sin(pitchRad);
    cameraForward.z = -Math::Cos(yawRad) * Math::Cos(pitchRad);  // Negative because -Z is forward
    cameraForward.Normalize();
    
    // Update game camera
    game->SetCameraPos(cameraPos);
    game->SetCameraForward(cameraForward);
    game->SetCameraYaw(yaw);
    game->SetCameraPitch(pitch);
}

// CubeActor implementation
CubeActor::CubeActor(Game* game, const Vector3& color,  int startingIndex)
: Actor(game)
, mMeshComponent(nullptr)
{
    Texture* texture = game->GetRenderer()->LoadTexture("./assets/textures/Blocks.png");
    // Get atlas from renderer cache
    TextureAtlas* atlas = game->GetRenderer()->LoadAtlas("./assets/textures/Blocks.json");
    
    // Get shared mesh from renderer cache (only one instance created)
    Mesh* mesh = game->GetRenderer()->LoadMesh("cube");

    mMeshComponent = new MeshComponent(this, *mesh, texture, atlas, startingIndex);
    mMeshComponent->SetColor(color);
}




PyramidActor::PyramidActor(Game* game, const Vector3& color,  int startingIndex)
: Actor(game)
, mMeshComponent(nullptr)
{

    // Get texture from renderer cache
    Texture* texture = game->GetRenderer()->LoadTexture("./assets/textures/Blocks.png");

    // Get atlas from renderer cache
    TextureAtlas* atlas = game->GetRenderer()->LoadAtlas("./assets/textures/Blocks.json");

    // Get shared mesh from renderer cache (only one instance created)
    Mesh* mesh = game->GetRenderer()->LoadMesh("pyramid");

    // Create mesh component with shared mesh and atlas
    mMeshComponent = new MeshComponent(this, *mesh, texture, atlas, startingIndex);
    mMeshComponent->SetColor(color);
}

void PyramidActor::OnUpdate(float deltaTime) {
 
}

void CubeActor::OnUpdate(float deltaTime){

}

// MarioActor implementation
MarioActor::MarioActor(Game* game)
: Actor(game)
, mSpriteComponent(nullptr)
{
    // Get atlas from renderer cache
    TextureAtlas* atlas = game->GetRenderer()->LoadAtlas("./assets/textures/Mario.json");
    
    // Get texture index from renderer cache
    Texture* texture = game->GetRenderer()->LoadTexture("./assets/textures/Mario.png");
    int textureIndex = game->GetRenderer()->GetTextureIndex(texture);
    
    // Create sprite component with atlas
    mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);
    
    // Setup running animation using atlas tile indices for Run1, Run2, Run3
    mSpriteComponent->AddAnimation("run",{"Run1.png","Run2.png","Run3.png"});
    mSpriteComponent->SetAnimation("run");
    mSpriteComponent->SetAnimFPS(8.0f);
}

void MarioActor::OnUpdate(float deltaTime) {

}



GoombaActor::GoombaActor(Game* game)
: Actor(game)
, mSpriteComponent(nullptr)
{
    // Get atlas from renderer cache
    TextureAtlas* atlas = game->GetRenderer()->LoadAtlas("./assets/textures/Goomba.json");
    
    // Get texture index from renderer cache
    Texture* texture = game->GetRenderer()->LoadTexture("./assets/textures/Goomba.png");
    int textureIndex = game->GetRenderer()->GetTextureIndex(texture);
    
    // Create sprite component with atlas
    mSpriteComponent = new SpriteComponent(this, textureIndex, atlas);

    // Setup running animation with Walk0 and Walk1 frames
    mSpriteComponent->AddAnimation("run", {"Walk0.png", "Walk1.png"});
    mSpriteComponent->SetAnimation("run");
    mSpriteComponent->SetAnimFPS(2.0f);
}

void GoombaActor::OnUpdate(float deltaTime) {
    // Update the sprite component
    mSpriteComponent->Update(deltaTime);
}


MIDIControlActor::MIDIControlActor(Game* game)
: Actor(game)
, mPrevSpacePressed(false)
, mPrevRPressed(false)
, mPrevJPressed(false)
, mPrevMPressed(false)
, mPrevUPressed(false)
, mPrevPlusPressed(false)
, mPrevMinusPressed(false)
, mPrevPPressed(false)
{
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

void MIDIControlActor::OnProcessInput(const Uint8* keyState)
{
    // SPACE - Play/Pause toggle
    bool spacePressed = keyState[SDL_SCANCODE_SPACE];
    if (spacePressed && !mPrevSpacePressed)
    {
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
    bool rPressed = keyState[SDL_SCANCODE_R];
    if (rPressed && !mPrevRPressed)
    {
        std::cout << "[MIDI] Reloading song..." << std::endl;
        // Thread-safe: loadSong acquires mutex
        MIDIPlayer::jumpTo(0.0f); // Reset position
    }
    mPrevRPressed = rPressed;
    
    // J - Jump to 30 seconds
    bool jPressed = keyState[SDL_SCANCODE_J];
    if (jPressed && !mPrevJPressed)
    {
        std::cout << "[MIDI] Jumping to 30 seconds..." << std::endl;
        // Thread-safe: jumpTo acquires mutex and resets positions
        MIDIPlayer::jumpTo(30.0f);
    }
    mPrevJPressed = jPressed;
    
    // M - Mute channel 0
    bool mPressed = keyState[SDL_SCANCODE_M];
    if (mPressed && !mPrevMPressed)
    {
        std::cout << "[MIDI] Muting channel 0..." << std::endl;
        // Thread-safe: muteChannel acquires mutex
        MIDIPlayer::muteChannel(0);
    }
    mPrevMPressed = mPressed;
    
    // U - Unmute channel 0
    bool uPressed = keyState[SDL_SCANCODE_U];
    if (uPressed && !mPrevUPressed)
    {
        std::cout << "[MIDI] Unmuting channel 0..." << std::endl;
        // Thread-safe: unmuteChannel acquires mutex
        MIDIPlayer::unmuteChannel(0);
    }
    mPrevUPressed = uPressed;
    
    // + (or =) - Increase speed
    bool plusPressed = keyState[SDL_SCANCODE_EQUALS] || keyState[SDL_SCANCODE_KP_PLUS];
    if (plusPressed && !mPrevPlusPressed)
    {
        std::cout << "[MIDI] Increasing speed to 1.5x..." << std::endl;
        // Thread-safe: setSpeed acquires mutex
        MIDIPlayer::setSpeed(1.5);
    }
    mPrevPlusPressed = plusPressed;
    
    // - Decrease speed
    bool minusPressed = keyState[SDL_SCANCODE_MINUS] || keyState[SDL_SCANCODE_KP_MINUS];
    if (minusPressed && !mPrevMinusPressed)
    {
        std::cout << "[MIDI] Decreasing speed to 0.5x..." << std::endl;
        // Thread-safe: setSpeed acquires mutex
        MIDIPlayer::setSpeed(0.5);
    }
    mPrevMinusPressed = minusPressed;
    
    // P - Print current channel states
    bool pPressed = keyState[SDL_SCANCODE_P];
    if (pPressed && !mPrevPPressed)
    {
        std::cout << "\n=== Current MIDI Channel States ===" << std::endl;
        // Thread-safe: getChannels returns reference but mutex protects access
        auto& channels = MIDIPlayer::getChannels();
        
        for (int i = 0; i < channels.size(); ++i)
        {
            const auto& channel = channels[i];
            
            if (!channel.active)
            {
                std::cout << "Channel " << i << ": INACTIVE" << std::endl;
                continue;
            }
            
            std::cout << "Channel " << i << ": ACTIVE" << std::endl;
            std::cout << "  Total notes: " << channel.notes.size() << std::endl;
            std::cout << "  Current position: " << channel.pos << "/" << channel.notes.size() << std::endl;
            
            // Show next few upcoming notes
            if (channel.pos < channel.notes.size())
            {
                std::cout << "  Next notes:" << std::endl;
                int notesToShow = std::min(5, static_cast<int>(channel.notes.size() - channel.pos));
                for (int j = 0; j < notesToShow; ++j)
                {
                    const auto& note = channel.notes[channel.pos + j];
                    std::cout << "    [" << (channel.pos + j) << "] "
                              << "Time: " << note.start << "s, "
                              << "Note: " << note.note << ", "
                              << (note.on ? "ON" : "OFF") << ", "
                              << "Vel: " << note.velocity << std::endl;
                }
            }
            else
            {
                std::cout << "  (All notes played)" << std::endl;
            }
        }
        std::cout << "===================================\n" << std::endl;
    }
    mPrevPPressed = pPressed;
}
