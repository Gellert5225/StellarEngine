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

v2f vertex vertexMain(uint vertexId                         [[vertex_id]],
                      device const VertexData* vertexData   [[buffer(0)]]) {
    v2f o;
    float4 pos = float4(vertexData[vertexId].position, 1.0, 1.0);
    o.position = pos;
    return o;
}

half4 fragment fragmentMain(v2f in [[stage_in]])  {
    //return half4(pow(in.color, 2.2), 1.0);
    return half4(1.0, 1.0, 1.0, 1.0);
}