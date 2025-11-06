#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

out vec2 fragTexCoord;

void main()
{
    // Simple passthrough for screen-space quad
    // Position is already in NDC space (-1 to 1)
    gl_Position = vec4(inPosition, 1.0);
    fragTexCoord = inTexCoord;
}
