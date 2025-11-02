#include "components/MeshComponent.hpp"
#include "actors/Actor.hpp"
#include "Game.hpp"
#include "render/Renderer.hpp"
#include "render/Mesh.hpp"
#include "render/TextureAtlas.hpp"


MeshComponent::MeshComponent(Actor* owner, Mesh& mesh, Texture* texture, TextureAtlas* textureAtlas, unsigned int startingIndex) :
    DrawComponent(owner)
    , mMesh(mesh)
    , mTexture(texture)
    , mTextureAtlas(textureAtlas)
    , mStartingIndex(startingIndex)
{
}

MeshComponent::~MeshComponent()
{
    // Don't delete the mesh - it's owned by someone else or cached
}

void MeshComponent::Draw(Renderer* renderer)
{
    // Draw method is now handled directly in Game.cpp
    // This method can be left empty or removed entirely
    (void)renderer;
}
