#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(set = 0, binding = 0) uniform GlobalUniforms {
    mat4 viewProjection;
    mat4 model;
} ubo;

layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = ubo.viewProjection * vec4(inPosition, 0.0, 1.0);
    fragColor = inColor;
}