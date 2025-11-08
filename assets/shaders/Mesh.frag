#version 330 core

// From vertex shader
in vec3 fragNormal;
in vec2 fragTexCoord;
flat in float fragTexIndex;         // Per vertex texturing
in vec3 fragColor;                  // Per instance color
flat in float fragTileIndex;        // Per instance tile index

uniform vec3 uDirectionalLightColor;    // Directional light color
uniform vec3 uAmbientLightColor;        // Ambient light color
uniform int uBloomPass;                 // 1 if rendering bloom pass, 0 otherwise

// Texture atlas uniforms
uniform sampler2D uTextureAtlas;
uniform vec2 uAtlasTileSize;
uniform int uAtlasColumns;

out vec4 outColor;

void main()
{   
    // If fragTileIndex is negative (e.g. -1) treat this as a uniformly colored object
    if (fragTileIndex < 0.0)
    {
        // Draw the object with the instance color only (opaque)
        outColor = vec4(fragColor, 1.0);
        return;
    }

    // Calculate tile index from instance tile index and per-vertex texture index
    int tileIndex = int(fragTileIndex) + int(fragTexIndex);

    // Calculate tile position in the atlas
    int tileX = tileIndex % uAtlasColumns;
    int tileY = tileIndex / uAtlasColumns;

    // Calculate UV offset for the tile
    vec2 tileOffset = vec2(float(tileX), float(tileY)) * uAtlasTileSize;

    // Scale the texture coordinates to fit within the tile
    vec2 scaledTexCoord = fragTexCoord * uAtlasTileSize;

    // Final UV coordinates in the atlas
    vec2 atlasUV = tileOffset + scaledTexCoord;

    // Sample from the texture atlas
    vec4 texColor = texture(uTextureAtlas, atlasUV);

    if(texColor.a < 0.1){
        discard;
    }

    vec3 baseColor = texColor.rgb * fragColor;
    
    // Flat shading: check if face is pointing toward light
    // TODO: Add lighting calculations here
    
    // If rendering bloom pass and object is not bloomed (indicated by fragColor.r < 0)
    // render as black to provide occlusion
    if (uBloomPass == 1 && fragColor.r < 0.0)
    {
        outColor = vec4(0.0, 0.0, 0.0, texColor.a);
        return;
    }
    
    outColor = vec4(baseColor, texColor.a);
}
