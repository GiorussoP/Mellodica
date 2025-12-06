#version 330 core

// From vertex shader
in vec3 fragNormal;
in vec2 fragTexCoord;
flat in float fragTexIndex;         // Per vertex texturing
in vec3 fragColor;                  // Per instance color
flat in float fragTileIndex;        // Per instance tile index
in vec3 fragWorldPos;               // World position for fog

uniform vec3 uDirectionalLightDir;          // Directional light direction
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

    // Use fractional part of texture coordinates for repeating within the tile
    vec2 repeatedTexCoord = fract(fragTexCoord);

    // Scale the repeated texture coordinates to fit within the tile
    vec2 scaledTexCoord = repeatedTexCoord * uAtlasTileSize;

    // Final UV coordinates in the atlas
    vec2 atlasUV = tileOffset + scaledTexCoord;

    // Sample from the texture atlas
    vec4 texColor = texture(uTextureAtlas, atlasUV);

    if(texColor.a < 0.1){
        discard;
    }


    vec3 baseColor =  texColor.rgb * fragColor;
    
    // Flat shading: check if face is pointing toward light
    float lightIntensity = 0.5f + 0.5f* dot(normalize(fragNormal), -uDirectionalLightDir);

    // Combine directional and ambient light
    vec3 lighting = uAmbientLightColor + (uDirectionalLightColor * lightIntensity);

    // Apply lighting to base color
    if(uBloomPass != 1 && uApplyLighting == 1){ 
            baseColor *= lighting;
    }


    // render as black to provide occlusion
    if (uBloomPass == 1 && fragColor.r < 0.0 && baseColor.r < 0.7 && baseColor.g < 0.7 && baseColor.b < 0.7)
    {
        outColor = vec4(0.0,0.0,0.0, texColor.a);
        return;
    }
    
    // Apply exponential fog for non-bloomed objects
    if (uBloomPass != 1)
    {
        float distance = length(fragWorldPos - uCameraPosition);
        float fogFactor = exp(-distance * uFogDensity);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        baseColor = mix(uFogColor, baseColor, fogFactor);
    }
    
    outColor = vec4(baseColor, texColor.a);
}
