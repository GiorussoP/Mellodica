#version 330 core

// From vertex shader
in vec3 fragNormal;
in vec2 fragTexCoord;
flat in float fragTexIndex;         // Per vertex texturing
in vec3 fragColor;                  // Per instance color
flat in float fragTileIndex;        // Per instance tile index (used as direct tile index for sprites)

uniform vec3 uDirectionalLightColor;    // Directional light color
uniform vec3 uAmbientLightColor;        // Ambient light color

// Texture atlas uniforms
uniform sampler2D uTextureAtlas;
uniform vec2 uAtlasTileSize;
uniform int uAtlasColumns;

out vec4 outColor;

void main()
{   
    // If fragTileIndex is negative (e.g. -1) treat this as a uniformly colored sprite
    if (fragTileIndex < 0.0)
    {
        outColor = vec4(fragColor, 1.0);
        return;
    }

    // For sprites, use the instance tile index directly
    int tileIndex = int(fragTileIndex);

    // Calculate tile position in the atlas
    int tileX = tileIndex % uAtlasColumns;
    int tileY = tileIndex / uAtlasColumns;

    // Calculate UV offset for the tile
    vec2 tileOffset = vec2(float(tileX), float(tileY)) * uAtlasTileSize;

    // Atlas texel size (in UV space)
    vec2 atlasTexel = 1.0 / vec2(textureSize(uTextureAtlas, 0));

    // Compute inner tile UV size by removing a one-texel border on each side
    vec2 innerTileSize = uAtlasTileSize - 2.0 * atlasTexel;

    // Final atlas UV: tileOffset + one-texel inset + scaled coordinate into the inner area
    vec2 atlasUV = tileOffset + atlasTexel + fragTexCoord * innerTileSize;

    // Sample from the texture atlas
    vec4 texColor = texture(uTextureAtlas, atlasUV);

    if(texColor.a < 0.1){
        discard;
    }

    vec3 baseColor = texColor.rgb * fragColor;
    
    // Sprites typically use full brightness or simple lighting
    // TODO: Add sprite-specific lighting calculations here if needed
    
    outColor = vec4(baseColor, texColor.a);
}
