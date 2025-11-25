#pragma once
#include "Component.hpp"
#include "Math.hpp"

class DrawComponent : public Component {
public:
  DrawComponent(class Actor *owner);
  ~DrawComponent();

  void SetVisible(bool visible) { mIsVisible = visible; }
  bool IsVisible() const { return mIsVisible; }

  void SetBloomed(bool bloomed) { mIsBloomed = bloomed; }
  bool IsBloomed() { return mIsBloomed; }

  void SetColor(Vector3 color) { mColor = color; }
  Vector3 &GetColor() { return mColor; }

  void SetOffset(Vector3 offset) { mOffset = offset; }
  Vector3 &GetOffset() { return mOffset; }

  void SetScale(Vector3 scale) { mScale = scale; }
  Vector3 &GetScale() { return mScale; }

protected:
  bool mIsVisible;
  bool mIsBloomed;
  Vector3 mColor;
  Vector3 mOffset;
  Vector3 mScale;
};
