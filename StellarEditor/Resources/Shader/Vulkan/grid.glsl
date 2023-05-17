#version 450 core
#pragma vert

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in float inTexIndex;
layout(location = 4) in float inTilingFactor;

layout(set = 0, binding = 0) uniform GlobalUniforms {
    mat4 viewProjection;
} ubo;

layout(push_constant) uniform Push {
    mat4 model;
} push;

layout(location = 0) out mat4 viewProjection;
layout(location = 4) out vec3 nearPoint;
layout(location = 5) out vec3 farPoint;

vec3 UnprojectPoint(float x, float y, float z, mat4 viewProjection) {
    vec4 unprojectedPoint =  inverse(viewProjection) * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    vec3 p = vec3(inPosition, 0.0);
    nearPoint = UnprojectPoint(p.x, p.y, 0.0, ubo.viewProjection).xyz; // unprojecting on the near plane
    farPoint = UnprojectPoint(p.x, p.y, 1.0, ubo.viewProjection).xyz; // unprojecting on the far plane
    gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
    viewProjection = ubo.viewProjection;
}

#version 450 core
#pragma frag

const float near = 0.1;
const float far = 100;

layout(location = 0) in mat4 viewProjection;
layout(location = 4) in vec3 nearPoint; // nearPoint calculated in vertex shader
layout(location = 5) in vec3 farPoint; // farPoint calculated in vertex shader

layout(location = 0) out vec4 outColor;
layout(location = 1) out float gl_FragDepth;

float scaleFactor(float scale) {
    return 0.77 * scale + 2.3;
}

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.35, 0.4, 0.4, (1.0 - min(line, 1.0)) / scaleFactor(scale));
    // z axis
    if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
        color = vec4(0.0, 0.0, 1.0, 1.0);
    // x axis
    if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
        color = vec4(1.0, 0.0, 0.0, 1.0);;
        
    return color;
}
float computeDepth(vec3 pos) {
    vec4 clip_space_pos = viewProjection * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}
float computeLinearDepth(vec3 pos) {
    vec4 clip_space_pos = viewProjection * vec4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (5.0 * near * far) / (far + near - clip_space_depth * (far - near));
    return linearDepth / far; 
}
void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

    outColor = (grid(fragPos3D, 1, true) + grid(fragPos3D, 10, true)) * float(t > 0);
    outColor.a *= fading;
    if (outColor.a == 0.0)
        discard;
}
