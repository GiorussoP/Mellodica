#version 330 core

in vec3 fragNormal;
in vec3 fragWorldPos;
in vec2 fragTexCoord;
in float fragTexIndex;

uniform vec3 uColor;
uniform vec3 uLightDir;        // Directional light direction
uniform vec3 uCameraPos;
uniform vec3 uAmbientLight;

// Texture atlas uniforms
uniform int uStartingAtlasIndex;
uniform sampler2D uTextureAtlas;
uniform vec2 uAtlasTileSize;
uniform int uAtlasColumns;

out vec4 outColor;

void main()
{
    // Normalize the normal vector
    vec3 normal = normalize(fragNormal);
    
    // Light direction (already a direction, no need to calculate from position)
    vec3 lightDir = normalize(-uLightDir);  // Negate because we want direction TO light
    
    // Calculate which tile to use from the atlas
    int tileIndex = uStartingAtlasIndex +  int(fragTexIndex);
    
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
    vec3 baseColor = texColor.rgb;
    
    // Calculate diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Toon shading - quantize the diffuse value into discrete steps
    float toonDiff;
    if (diff > 0.95)
        toonDiff = 1.0;
    else if (diff > 0.5)
        toonDiff = 0.7;
    else if (diff > 0.25)
        toonDiff = 0.4;
    else
        toonDiff = 0.2;
    
    // Calculate view direction for rim lighting
    vec3 viewDir = normalize(uCameraPos - fragWorldPos);
    float rim = 1.0 - max(dot(viewDir, normal), 0.0);
    rim = smoothstep(0.6, 1.0, rim);
    
    // Ambient component
    vec3 ambient = uAmbientLight * baseColor;
    
    // Diffuse component with toon shading
    vec3 diffuse = toonDiff * baseColor;
    
    // Rim light (outline effect)
    vec3 rimColor = vec3(0.8, 0.8, 1.0) * rim * 0.5;
    
    // Combine all components
    vec3 result = ambient + diffuse + rimColor;
    
    outColor = vec4(result, 1.0);
}

