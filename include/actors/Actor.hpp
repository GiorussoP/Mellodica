#pragma once
#include <vector>
#include <SDL2/SDL.h>
#include "Math.hpp"

enum class ActorState
{
    Active,
    Paused,
    Destroy
};

class Actor
{
public:
    Actor(class Game* game);
    virtual ~Actor();
    
    // Update function called from Game (not overridable)
    void Update(float deltaTime);
    
    // ProcessInput function called from Game (not overridable)
    void ProcessInput(const Uint8* keyState);
    
    // Position getter/setter
    const Vector3& GetPosition() const { return mPosition; }
    void SetPosition(const Vector3 pos);
    
    // Scale getter/setter
    Vector3 GetScale() const { return mScale; }
    void SetScale(const Vector3 scale) { mScale = scale; }
    
    // Rotation getter/setter (for 2D sprites, rotation around Y axis)
    Quaternion GetRotation() const { return mRotation; }
    void SetRotation(const Quaternion rotation) { mRotation = rotation; }
    
    // State getter/setter
    ActorState GetState() const { return mState; }
    void SetState(ActorState state) { mState = state; }
    
    // Game getter
    class Game* GetGame() { return mGame; }
    
    // Components getter
    const std::vector<class Component*>& GetComponents() const { return mComponents; }
    
    // Returns component of type T, or null if doesn't exist
    template <typename T>
    T* GetComponent() const
    {
        for (auto c : mComponents)
        {
            T* t = dynamic_cast<T*>(c);
            if (t != nullptr)
            {
                return t;
            }
        }
        return nullptr;
    }

protected:
    class Game* mGame;
    
    // Any actor-specific update code (overridable)
    virtual void OnUpdate(float deltaTime);
    
    // Any actor-specific input code (overridable)
    virtual void OnProcessInput(const Uint8* keyState);
    
    // Actor's state
    ActorState mState;
    
    // Transform
    Vector3 mPosition;
    Vector3 mScale;
    Quaternion mRotation;
    
    // Components
    std::vector<class Component*> mComponents;

private:
    friend class Component;
    
    // Adds component to Actor (called automatically in component constructor)
    void AddComponent(class Component* c);
};
