#pragma once
#include "DrawComponent.hpp"
#include "Math.hpp"
#include "Mesh.hpp"
#include "TextureAtlas.hpp"
#include <vector>

class MeshComponent : public DrawComponent
{
public:
    MeshComponent(class Actor* owner, Mesh& mesh, Texture* texture = nullptr, TextureAtlas* textureAtlas = nullptr, unsigned int startingIndex = 0);
    ~MeshComponent();
    
    Mesh& GetMesh() const { return mMesh;}
    TextureAtlas* GetTextureAtlas() const { return mTextureAtlas; }
    unsigned int GetStartingIndex() const { return mStartingIndex; }
protected:
    Mesh& mMesh;
    Texture* mTexture;
    TextureAtlas* mTextureAtlas;
    unsigned int mStartingIndex;
};
