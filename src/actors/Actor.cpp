#include "actors/Actor.hpp"
#include "Game.hpp"
#include "components/Component.hpp"
#include "ChunkGrid.hpp"
#include <algorithm>

Actor::Actor(Game* game)
: mGame(game)
, mState(ActorState::Active)
, mPosition(Vector3::Zero)
, mScale(1.0f)
, mRotation(Quaternion::Identity)
{
    mGame->AddActor(this);
    
    // Register with spatial grid at default position (0,0,0)
    // Only register if spatial grid exists
    auto* grid = mGame->GetChunkGrid();
    if (grid)
    {
        grid->RegisterActor(this);
    }
}

Actor::~Actor()
{
    mGame->RemoveActor(this);
    
    // Delete components
    for (auto comp : mComponents)
    {
        delete comp;
    }
    mComponents.clear();
}

void Actor::Update(float deltaTime)
{
    if (mState == ActorState::Active)
    {
        // Update components
        for (auto comp : mComponents)
        {
            if (comp->IsEnabled())
            {
                comp->Update(deltaTime);
            }
        }
        
        // Actor-specific update
        OnUpdate(deltaTime);
    }
}

void Actor::OnUpdate(float deltaTime)
{
    (void)deltaTime;
}

void Actor::ProcessInput(const Uint8* keyState)
{
    if (mState == ActorState::Active)
    {
        // Process components input
        for (auto comp : mComponents)
        {
            if (comp->IsEnabled())
            {
                comp->ProcessInput(keyState);
            }
        }
        
        // Actor-specific input
        OnProcessInput(keyState);
    }
}

void Actor::OnProcessInput(const Uint8* keyState)
{
    (void)keyState;
}

void Actor::AddComponent(Component* c)
{
    // Insert component maintaining update order
    int order = c->GetUpdateOrder();
    auto iter = mComponents.begin();
    for (; iter != mComponents.end(); ++iter)
    {
        if (order < (*iter)->GetUpdateOrder())
        {
            break;
        }
    }
    
    mComponents.insert(iter, c);
}

void Actor::SetPosition(const Vector3& pos)
{
    mPosition = pos;

    // Automatically update chunk grid when position changes
    mGame->GetChunkGrid()->UpdateActor(this);
}
