#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

// HUD positioning uniforms (in NDC space)
uniform vec2 uNDCPosition;  // Center position in NDC (-1 to 1)
uniform vec2 uNDCScale;     // Size in NDC space

out vec2 fragTexCoord;

void main()
{
    // Transform the sprite quad vertex to the HUD position and scale
    // inPosition is in range [-0.5, 0.5] (sprite quad is centered)
    vec2 transformedPos = inPosition.xy * uNDCScale + uNDCPosition;
    
    gl_Position = vec4(transformedPos, 0.0, 1.0);
    fragTexCoord = inTexCoord;
}
