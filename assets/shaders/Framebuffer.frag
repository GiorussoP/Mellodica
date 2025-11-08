#version 330 core

in vec2 fragTexCoord;

uniform sampler2D uFramebufferTexture;
uniform sampler2D uBloomTexture;
uniform bool uIsDark;

out vec4 outColor;

void main()
{
    // Sample the main framebuffer texture
    vec3 sceneColor = texture(uFramebufferTexture, fragTexCoord).rgb;
    
    // Sample the bloom texture (already blurred)
    vec3 bloomColor = texture(uBloomTexture, fragTexCoord).rgb;
    
    // Additive blending of bloom
    vec3 result;
    if (uIsDark){
        result = (bloomColor == vec3(0.0)) ?  vec3(0.0) : (sceneColor * bloomColor) + bloomColor;
    }
    else {
        result = sceneColor + bloomColor;
    }
    outColor = vec4(result, 1.0);
}
