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
    if (!mIsVisible) {
        return;
    }
    
    // Get actor's transform
    Vector3 position = mOwner->GetPosition();
    float scale = mOwner->GetScale();
    float rotation = mOwner->GetRotation();
    
    // Convert rotation to quaternion (rotation around Y-axis for 3D)
    Quaternion quat(Vector3::UnitY, rotation);
    
    // Draw the mesh using atlas parameters
    // The per-triangle texture indices are already baked into the mesh vertex data
    // but can be remapped using the mTextureIndexRemap array
    renderer->DrawMesh(
        *this,
        position,
        Vector3(scale, scale, scale),
        quat,
        RendererMode::TRIANGLES
    );
}
