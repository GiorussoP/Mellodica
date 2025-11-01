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
uniform int uAtlasColumns;
uniform int uTileIndex;  // Starting index
uniform vec2 uTileOffset;  // Actual UV offset for the tile (for sprites)


uniform bool uApplyLighting;
uniform bool uIsSprite;  // Indicates if rendering a sprite

out vec4 outColor;

void main()
{   
    vec2 atlasUV;
    if(uIsSprite){
        vec2 scaledTexCoord = fragTexCoord * uAtlasTileSize;
        atlasUV = uTileOffset + scaledTexCoord;
    }
    else {
        int tileIndex = uTileIndex + int(fragTexIndex);


    
        // Calculate tile position in the atlas
        int tileX = tileIndex % uAtlasColumns;
        int tileY = tileIndex / uAtlasColumns;
        
        // Calculate UV offset for the tile
        vec2 tileOffset = vec2(float(tileX), float(tileY)) * uAtlasTileSize;
        
        // Scale the texture coordinates to fit within the tile
        vec2 scaledTexCoord = fragTexCoord * uAtlasTileSize;
        
        // Final UV coordinates in the atlas
        atlasUV = tileOffset + scaledTexCoord;
    }



    
    // Sample from the texture atlas
    vec4 texColor = texture(uTextureAtlas, atlasUV);

    if(texColor.a < 0.1){
        discard;
    }



    vec3 baseColor = texColor.rgb;
    
    // Flat shading: check if face is pointing toward light
    //float diff = clamp(dot(normal, lightDir),0.0,1.0);



    
    //vec3 result = baseColor * (uLightColor * diff + (1.0-diff) * uAmbientLight);

    outColor = uTileIndex >= 0.0 ? vec4(baseColor, texColor.a) : vec4(baseColor,1.0);
}
