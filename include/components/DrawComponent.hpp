#pragma once
#include "Component.hpp"
#include "Math.hpp"

class DrawComponent : public Component
{
public:
    DrawComponent(class Actor* owner);
    ~DrawComponent();
    
    virtual void Draw(class Renderer* renderer);
    
    void SetVisible(bool visible) { mIsVisible = visible; }
    
    void SetColor(const Vector3& color) { mColor = color; }
    const Vector3& GetColor() const { return mColor; }
    
protected:
    bool mIsVisible;
    Vector3 mColor;
};
