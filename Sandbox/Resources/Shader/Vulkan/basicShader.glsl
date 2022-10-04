#version 450 core
#pragma vert

layout(location = 0) in vec2 inPosition;

layout(set = 0, binding = 0) uniform GlobalUniforms {
    mat4 viewProjection;
} ubo;

layout(push_constant) uniform Push {
    mat4 model;
    vec3 color;
} push;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubo.viewProjection * push.model * vec4(inPosition, 0.0, 1.0);
    fragColor = push.color;
}

#version 450 core
#pragma frag

precision mediump float;

layout(location = 0) in vec3 fragColor;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(fragColor, 1.0);
}