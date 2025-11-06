#version 330 core

in vec2 fragTexCoord;

uniform sampler2D uFramebufferTexture;

out vec4 outColor;

void main()
{
    // Sample the framebuffer texture and display it
    outColor = texture(uFramebufferTexture, fragTexCoord);
}
