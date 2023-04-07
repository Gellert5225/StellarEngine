#version 450 core
#pragma vert

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in float inTilingFactor;

layout(set = 1, binding = 0) uniform GlobalUniforms {
    mat4 viewProjection;
} ubo;

layout(push_constant) uniform Push {
    mat4 model;
    vec4 color;
} push;

layout(location = 0) out vec4 fragColor;
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

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in float fragTilingFactor;

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = texture(texSampler, fragTexCoord * fragTilingFactor) * fragColor;
}