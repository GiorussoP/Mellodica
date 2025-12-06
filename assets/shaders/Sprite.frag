#version 330 core

// From vertex shader
in vec3 fragNormal;
in vec2 fragTexCoord;
flat in float fragTexIndex;         // Per vertex texturing
in vec3 fragColor;                  // Per instance color
flat in float fragTileIndex;        // Per instance tile index (used as direct tile index for sprites)
in vec2 spriteSize;
in vec3 fragWorldPos;               // World position for fog


uniform vec3 uDirectionalLightColor;    // Directional light color
uniform vec3 uAmbientLightColor;        // Ambient light color
uniform int uBloomPass;                 // 1 if rendering bloom pass, 0 otherwise
uniform int uApplyLighting;             // 1 if lighting should be applied, 0 otherwise
uniform vec3 uCameraPosition;           // Camera world position for fog
uniform vec3 uFogColor;                 // Fog color
uniform float uFogDensity;              // Fog density

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
    vec2 innerTileSize = (uAtlasTileSize - 2.0 * atlasTexel) / spriteSize;

    // Final atlas UV: tileOffset + one-texel inset + scaled coordinate into the inner area
    vec2 atlasUV = tileOffset + atlasTexel + fragTexCoord * innerTileSize;

    // Sample from the texture atlas
    vec4 texColor = texture(uTextureAtlas, atlasUV);

    if(texColor.a < 0.1){
        discard;
    }

    vec3 baseColor = texColor.rgb * fragColor;
    
    // Apply lighting to sprites (simple ambient + directional)
    // Sprites are billboards, so we use a simple lighting model
    if (uApplyLighting == 1 && uBloomPass != 1) {
        vec3 ambient = uAmbientLightColor * baseColor;
        vec3 diffuse = uDirectionalLightColor * baseColor * 0.5; // Reduced intensity for sprites
        vec3 litColor = ambient + diffuse;
        baseColor = litColor;

        // Apply 
        float distance = length(fragWorldPos);
        float fogFactor = exp(-distance * 2.0f * uFogDensity);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        baseColor = mix(uFogColor, baseColor, fogFactor);
    }
    
    // If rendering bloom pass and object is not bloomed (indicated by fragColor.r < 0)
    // render as black to provide occlusion
    if (uBloomPass == 1 && fragColor.r < 0.0)
    {
        outColor = vec4(0.0, 0.0, 0.0, texColor.a);
        return;
    }
    
    outColor = vec4(baseColor, texColor.a);
}
