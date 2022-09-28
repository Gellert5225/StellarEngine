#version 450 core
#pragma vert

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in float inTilingFactor;

layout(set = 0, binding = 0) uniform GlobalUniforms {
    mat4 viewProjection;
} ubo;

layout(push_constant) uniform Push {
    mat4 model;
    vec3 color;
} push;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out float fragTilingFactor;

void main() {
    gl_Position = ubo.viewProjection * push.model * vec4(inPosition, 0.0, 1.0);
    fragColor = push.color;
    fragTexCoord = inTexCoord;
    fragTilingFactor = inTilingFactor;
}

#version 450 core
#pragma frag

precision mediump float;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float fragTilingFactor;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    float r = pow(fragColor.r * texture(texSampler, fragTexCoord).r, 1.0/2.2);
    float g = pow(fragColor.g * texture(texSampler, fragTexCoord).g, 1.0/2.2);
    float b = pow(fragColor.b * texture(texSampler, fragTexCoord).b, 1.0/2.2);

    outColor = vec4(r, g, b, 1.0);
}