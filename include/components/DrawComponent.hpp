#pragma once
#include "Component.hpp"
#include "Math.hpp"

class DrawComponent : public Component
{
public:
    DrawComponent(class Actor* owner);
    ~DrawComponent();
    
    
    void SetVisible(bool visible) { mIsVisible = visible; }
    bool IsVisible() const { return mIsVisible; }
    

    void SetBloomed(bool bloomed) { mIsBloomed = bloomed; }
    bool IsBloomed() const { return mIsBloomed; }
    
    void SetColor(const Vector3& color) { mColor = color; }
    const Vector3& GetColor() const { return mColor; }
    
protected:
    bool mIsVisible;
    bool mIsBloomed;
    Vector3 mColor;
};
