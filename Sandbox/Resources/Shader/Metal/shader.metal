#include <metal_stdlib>
using namespace metal;

struct v2f {
    float4 position [[position]];
    half3 color;
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

v2f vertex vertexMain(uint vertexId                         [[vertex_id]],
                      device const VertexData* vertexData   [[buffer(0)]],
                      device const GlobalUniforms& uniform  [[buffer(1)]],
                      constant Push& push                   [[buffer(2)]]) {
    v2f o;
    float4 pos = float4(vertexData[vertexId].position, 0.0, 1.0);
    pos = uniform.viewProjection * push.model * pos;
    o.position = pos;
    return o;
}

half4 fragment fragmentMain(v2f in [[stage_in]])  {
    return half4(1.0, 1.0, 1.0, 1.0);
}
