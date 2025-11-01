
#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in float inTexIndex;

uniform mat4 uWorldTransform;
uniform mat4 uNormalMatrix; // Transform for normals


out vec3 fragNormal;
out vec2 fragTexCoord;
flat out float fragTexIndex;

void main()
{
    // Transform position by combined MVP matrix
    gl_Position = uWorldTransform * vec4(inPosition, 1.0);
    

    // Transform normal to world space (using normal matrix to handle non-uniform scaling)
    fragNormal = mat3(uNormalMatrix) * inNormal;
    

    // Pass texture coordinates to fragment shader
    fragTexCoord = inTexCoord;
    
    // Pass texture index to fragment shader
    fragTexIndex = inTexIndex;
}
