#version 330 core

// From vertex shader
in vec3 fragNormal;
in vec2 fragTexCoord;
flat in float fragTexIndex;         // Per vertex texturing

uniform vec3 uColor;                    // Object color
uniform vec3 uDirectionalLightColor;    // Directional light color
uniform vec3 uAmbientLightColor;        // Ambient light color

// Texture atlas uniforms
uniform sampler2D uTextureAtlas;
uniform vec2 uAtlasTileSize;
uniform vec2 uTileOffset;  // Actual UV offset for the tile (for sprites)

out vec4 outColor;

void main()
{   
    // For sprites, use the precomputed tile offset
    vec2 scaledTexCoord = fragTexCoord * uAtlasTileSize;
    vec2 atlasUV = uTileOffset + scaledTexCoord;
    
    // Sample from the texture atlas
    vec4 texColor = texture(uTextureAtlas, atlasUV);

    if(texColor.a < 0.1){
        discard;
    }

    vec3 baseColor = texColor.rgb;
    
    // Sprites typically use full brightness or simple lighting
    // TODO: Add sprite-specific lighting calculations here if needed
    
    outColor = vec4(baseColor, texColor.a);
}
