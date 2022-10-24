#include <metal_stdlib>
using namespace metal;

struct v2f {
    float4 position [[position]];
    half3 color;
    float2 texCoord;
    float4x4 viewProjection;
    float3 nearPoint;
    float3 farPoint;
};

struct InstanceData {
    float4x4 instanceTransform;
    float4 instanceColor;
};

struct VertexData {
    packed_float2 position;
    packed_float2 texCoord;
    float tilingFactor;
};

struct GlobalUniforms {
    float4x4 viewProjection;
};

struct Push {
    float4x4 model;
    packed_float3 color;
};

float3 UnprojectPoint(float x, float y, float z, float4x4 viewProjection) {
    float4 unprojectedPoint = inverse(viewProjection) * float4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

v2f vertex vertexMain(uint vertexId                         [[vertex_id]],
                      device const VertexData* vertexData   [[buffer(0)]],
                      device const GlobalUniforms& uniform  [[buffer(1)]],
                      constant Push& push                   [[buffer(2)]]) {
    v2f o;

    float3 p = float3(vertexData[vertexId].position, 0.0);
    o.nearPoint = UnprojectPoint(p.x, p.y, 0.0, uniform.viewProjection).xyz;
    o.farPoint = UnprojectPoint(p.x, p.y, 1.0, uniform.viewProjection).xyz;
    o.position = float4(p, 1.0);
    o.color = half3(push.color);
    o.texCoord = vertexData[vertexId].texCoord;
    o.viewProjection = uniform.viewProjection;

    return o;
}

constant float near = 0.1f;
constant float far = 100;

float scaleFactor(float scale) {
    return 0.77 * scale + 2.3;
}

float4 grid(float3 fragPos3D, float scale, bool drawAxis) {
    float2 coord = fragPos3D.xz * scale;
    float2 derivative = fwidth(coord);
    float2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    float4 color = float4(0.35, 0.4, 0.4, (1.0 - min(line, 1.0)) / scaleFactor(scale));
    // z axis
    if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
        color = float4(0.0, 0.0, 1.0, 1.0);
    // x axis
    if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
        color = float4(1.0, 0.0, 0.0, 1.0);;
        
    return color;
}

float computeDepth(float3 pos, float4x4 vp) {
    float4 clip_space_pos = vp * float4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}

float computeLinearDepth(float3 pos, float4x4 vp) {
    float4 clip_space_pos = vp * float4(pos.xyz, 1.0);
    float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (5.0 * near * far) / (far + near - clip_space_depth * (far - near));
    return linearDepth / far; 
}

half4 fragment fragmentMain(v2f in [[stage_in]],
                            texture2d<half, access::sample> tex [[texture(0)]]) {
    float t = -in.nearPoint.y / (in.farPoint.y - in.nearPoint.y);
    float3 fragPos3D = in.nearPoint + t * (in.farPoint - in.nearPoint);

    //gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D, in.viewProjection);
    float fading = max(0, (0.5 - linearDepth));

    outColor = (grid(fragPos3D, 1, true) + grid(fragPos3D, 10, true)) * float(t > 0);
    outColor.a *= fading;
    return half4(outColor);
}
