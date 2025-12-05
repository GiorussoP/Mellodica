#pragma once
#include "DrawComponent.hpp"
#include "Math.hpp"
#include "Mesh.hpp"
#include "TextureAtlas.hpp"
#include <vector>

class MeshComponent : public DrawComponent {
public:
  MeshComponent(class Actor *owner, Mesh &mesh, Texture *texture = nullptr,
                TextureAtlas *textureAtlas = nullptr, int startingIndex = -1);
  ~MeshComponent();

  Mesh &GetMesh() const { return mMesh; }
  TextureAtlas *GetTextureAtlas() const { return mTextureAtlas; }
  int GetStartingIndex() const { return mStartingIndex; }
  void SetTextureIndex(int index) { mStartingIndex = index; }

  void SetRelativeRotation(Quaternion relRot) { mRelativeRotation = relRot; }
  Quaternion &GetRelativeRotation() { return mRelativeRotation; }

protected:
  Quaternion mRelativeRotation;
  Mesh &mMesh;
  Texture *mTexture;
  TextureAtlas *mTextureAtlas;
  int mStartingIndex;
};
