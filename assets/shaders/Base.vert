
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

void main()
{
    // Transform position by instance model then view-projection
    gl_Position = uViewProjection * inInstanceModel * vec4(inPosition, 1.0);
    
    // Transform normal to world space using instance normal matrix
    fragNormal = mat3(inInstanceNormal) * inNormal;
    
    // Pass texture coordinates to fragment shader
    fragTexCoord = inTexCoord;
    
    // Pass texture index to fragment shader
    fragTexIndex = inTexIndex;
    
    // Pass instance color and tile index
    fragColor = inInstanceColor;
    fragTileIndex = inInstanceTileIndex;
}
