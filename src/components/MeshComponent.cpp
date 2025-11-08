#include "components/MeshComponent.hpp"
#include "actors/Actor.hpp"
#include "Game.hpp"
#include "render/Renderer.hpp"
#include "render/Mesh.hpp"
#include "render/TextureAtlas.hpp"


MeshComponent::MeshComponent(Actor* owner, Mesh& mesh, Texture* texture, TextureAtlas* textureAtlas, int startingIndex) :
    DrawComponent(owner)
    , mMesh(mesh)
    , mTexture(texture)
    , mTextureAtlas(textureAtlas)
    , mStartingIndex(startingIndex)
{
}

MeshComponent::~MeshComponent()
{
    
}

