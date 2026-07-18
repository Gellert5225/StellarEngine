#version 450 core
#pragma vert

layout(location = 0) in vec2 inPosition;

layout(set = 0, binding = 0) uniform GlobalUniforms {
    mat4 viewProjection;
} ubo;

layout(push_constant) uniform Push {
    mat4 model;
} push;

layout(location = 0) out mat4 vViewProjection; // occupies locations 0..3
layout(location = 4) out vec3 nearPoint;
layout(location = 5) out vec3 farPoint;

vec3 UnprojectPoint(float x, float y, float z) {
    vec4 unprojected = inverse(ubo.viewProjection) * vec4(x, y, z, 1.0);
    return unprojected.xyz / unprojected.w;
}

void main() {
    // inPosition is a full-screen quad already in clip space (-1..1).
    nearPoint = UnprojectPoint(inPosition.x, inPosition.y, 0.0); // near plane
    farPoint  = UnprojectPoint(inPosition.x, inPosition.y, 1.0); // far plane
    vViewProjection = ubo.viewProjection;
    // push.model is identity for the grid; referenced only so the push-constant
    // that Renderer::RenderGeometry sends stays part of the pipeline layout.
    gl_Position = push.model * vec4(inPosition, 0.0, 1.0);
}

#version 450 core
#pragma frag

const float near = 0.1;
const float far = 100.0;

layout(location = 0) in mat4 vViewProjection; // occupies locations 0..3
layout(location = 4) in vec3 nearPoint;
layout(location = 5) in vec3 farPoint;

layout(location = 0) out vec4 outColor;

float scaleFactor(float scale) {
    return 0.77 * scale + 2.3;
}

vec4 grid(vec3 fragPos3D, float scale) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 gridLine = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(gridLine.x, gridLine.y);
    float minimumz = min(derivative.y, 1.0);
    float minimumx = min(derivative.x, 1.0);
    vec4 color = vec4(0.9, 0.9, 0.9, (1.0 - min(line, 1.0)) / scaleFactor(scale));
    // z axis (blue)
    if (fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
        color = vec4(0.0, 0.0, 1.0, 1.0);
    // x axis (red)
    if (fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
        color = vec4(1.0, 0.0, 0.0, 1.0);
    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip = vViewProjection * vec4(pos, 1.0);
    return clip.z / clip.w;
}

// Distance at which the grid has fully faded out (world units).
const float GRID_FADE_DISTANCE = 80.0;

void main() {
    // Intersect the camera ray with the y = 0 plane (ground).
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    if (t <= 0.0)
        discard; // above the horizon: no ground plane here

    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    // Fade by view distance (nearPoint ~= camera). Convention-independent, and it
    // kills the horizon line / far white sheet where cells go sub-pixel.
    float dist = length(fragPos3D - nearPoint);
    float fading = clamp(1.0 - dist / GRID_FADE_DISTANCE, 0.0, 1.0);
    if (fading <= 0.0)
        discard; // beyond the grid extent: no ground, see straight through

    // The WHOLE ground plane writes depth (not just the visible lines), so objects
    // below it are cleanly occluded and grid lines never punch anti-aliased holes
    // into objects in front. Between-line fragments stay (alpha ~0) on purpose --
    // they contribute no color but do write depth.
    gl_FragDepth = computeDepth(fragPos3D);

    outColor = grid(fragPos3D, 1.0);   // 1-unit lines + colored axes
    outColor.a *= fading;
}
