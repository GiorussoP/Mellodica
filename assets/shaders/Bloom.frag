#version 330 core

in vec2 fragTexCoord;

uniform sampler2D uTexture;
uniform bool uHorizontal;
uniform float uWeights[5];

out vec4 outColor;

void main()
{
    // Size of one texel
    vec2 texelSize = 1.0 / vec2(textureSize(uTexture, 0));
    
    // Blur radius in pixels (increase for wider bloom)
    float blurRadius = 50.0;
    
    // Number of samples (higher = smoother, more expensive)
    const int samples = 15;
    
    // Current fragment color with center weight
    vec3 result = texture(uTexture, fragTexCoord).rgb * 0.2;
    float totalWeight = 0.1;
    
    if (uHorizontal)
    {
        // Horizontal blur - sample along X axis
        for (int i = 1; i <= samples; ++i)
        {
            float offset = (float(i) / float(samples)) * blurRadius;
            float weight = 1.0 / (1.0 + offset * offset * 0.5);  // Gaussian-like falloff
            
            result += texture(uTexture, fragTexCoord + vec2(texelSize.x * offset, 0.0)).rgb * weight;
            result += texture(uTexture, fragTexCoord - vec2(texelSize.x * offset, 0.0)).rgb * weight;
            totalWeight += weight * 2.0;
        }
    }
    else
    {
        // Vertical blur - sample along Y axis
        for (int i = 1; i <= samples; ++i)
        {
            float offset = (float(i) / float(samples)) * blurRadius;
            float weight = 1.0 / (1.0 + offset * offset * 0.5);  // Gaussian-like falloff
            
            result += texture(uTexture, fragTexCoord + vec2(0.0, texelSize.y * offset)).rgb * weight;
            result += texture(uTexture, fragTexCoord - vec2(0.0, texelSize.y * offset)).rgb * weight;
            totalWeight += weight * 2.0;
        }
    }
    
    // Normalize by total weight
    outColor = vec4(result / totalWeight, 1.0);
}
