
#version 330 core

// Per-vertex attributes
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in float inTexIndex;

// Per-instance attributes (mat4 takes 4 locations)
layout(location = 4) in mat4 inInstanceModel;      // Locations 4, 5, 6, 7
layout(location = 8) in mat4 inInstanceNormal;     // Locations 8, 9, 10, 11
layout(location = 12) in vec3 inInstanceColor;     // Location 12
layout(location = 13) in float inInstanceTileIndex; // Location 13

uniform mat4 uViewProjection;

out vec3 fragNormal;
out vec2 fragTexCoord;
flat out float fragTexIndex;
out vec3 fragColor;
flat out float fragTileIndex;
out vec2 spriteSize;

void main()
{
    // Transform position by instance model then view-projection
    gl_Position = uViewProjection * inInstanceModel * vec4(inPosition, 1.0);
    
    // Transform normal to world space using instance normal matrix
    fragNormal = mat3(inInstanceNormal) * inNormal;
    
    // Extract scale from model matrix
    vec3 scale = vec3(length(inInstanceModel[0].xyz), length(inInstanceModel[1].xyz), length(inInstanceModel[2].xyz));
    spriteSize = scale.xy;
    // Scale texture coordinates based on the face normal direction
    vec3 aN = abs(inNormal);
  
    // Facing X: scale UV by (Y, Z)
    fragTexCoord = (aN.x * inTexCoord * vec2(scale.z, scale.y)) +
                        (aN.y * inTexCoord * vec2(scale.x, scale.z)) +
                        (aN.z * inTexCoord * vec2(scale.x, scale.y));
    
    // Pass texture index to fragment shader
    fragTexIndex = inTexIndex;
    
    // Pass instance color and tile index
    fragColor = inInstanceColor;
    fragTileIndex = inInstanceTileIndex;
}
