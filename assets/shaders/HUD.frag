#version 330 core

// From vertex shader (Screenspace.vert provides these)
in vec2 fragTexCoord;

// HUD texture atlas uniforms
uniform sampler2D uHUDTexture;
uniform vec2 uAtlasTileSize;
uniform int uAtlasColumns;
uniform int uTileIndex;  // Which tile to display from the atlas
uniform vec4 uTintColor; // Color tint/modulation (default white)

uniform bool uHasTexture;

out vec4 outColor;

void main()
{   

    if(!uHasTexture){
        outColor = uTintColor;
        return;
    }

    // If tileIndex is negative, treat as a full texture sample (no atlas)
    if (uTileIndex < 0)
    {
        vec4 texColor = texture(uHUDTexture, fragTexCoord);
        outColor = texColor * uTintColor;
        return;
    }

    // Calculate tile position in the atlas
    int tileX = uTileIndex % uAtlasColumns;
    int tileY = uTileIndex / uAtlasColumns;

    // Calculate UV offset for the tile
    vec2 tileOffset = vec2(float(tileX), float(tileY)) * uAtlasTileSize;

    // Atlas texel size (in UV space)
    vec2 atlasTexel = 1.0 / vec2(textureSize(uHUDTexture, 0));

    // Compute inner tile UV size by removing a one-texel border on each side
    vec2 innerTileSize = uAtlasTileSize - 2.0 * atlasTexel;

    // Final atlas UV: tileOffset + one-texel inset + scaled coordinate into the inner area
    vec2 atlasUV = tileOffset + atlasTexel + fragTexCoord * innerTileSize;

    // Sample from the texture atlas
    vec4 texColor = texture(uHUDTexture, atlasUV);

    // Discard fully transparent pixels
    if(texColor.a < 0.1){
        discard;
    }

    // Apply tint color
    outColor = texColor * uTintColor;
}
