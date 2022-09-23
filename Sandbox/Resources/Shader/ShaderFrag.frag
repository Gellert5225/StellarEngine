#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    float r = pow(texture(texSampler, fragTexCoord).r, 1/2.2);
    float g = pow(texture(texSampler, fragTexCoord).g, 1/2.2);
    float b = pow(texture(texSampler, fragTexCoord).b, 1/2.2);

    outColor = vec4(r, g, b, 1.0);
}