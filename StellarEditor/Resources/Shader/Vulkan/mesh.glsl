#version 450 core
#pragma vert

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(set = 0, binding = 0) uniform GlobalUniforms {
    mat4 viewProjection;
} ubo;

layout(push_constant) uniform Push {
    mat4 model;
} push;

layout(location = 0) out vec3 vNormal;
layout(location = 1) out vec2 vUV;

void main() {
    // mat3(model) is fine for rotation/uniform-scale; use the normal matrix
    // (transpose(inverse(mat3(model)))) once you have non-uniform scaling.
    vNormal = mat3(push.model) * inNormal;
    vUV = inUV;
    gl_Position = ubo.viewProjection * push.model * vec4(inPosition, 1.0);
}

#version 450 core
#pragma frag

layout(location = 0) in vec3 vNormal;
layout(location = 1) in vec2 vUV;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 n = normalize(vNormal);
    vec3 lightDir = normalize(vec3(0.4, 1.0, 0.6));
    float diffuse = max(dot(n, lightDir), 0.0);
    float shade = diffuse * 0.8 + 0.2;          // + ambient so back faces aren't black
    outColor = vec4(vec3(shade), 1.0);
    // debug alternative: outColor = vec4(n * 0.5 + 0.5, 1.0);  // normals as color
}
