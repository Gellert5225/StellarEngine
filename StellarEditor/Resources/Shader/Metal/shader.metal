#include <metal_stdlib>
using namespace metal;

struct v2f {
    float4 position [[position]];
	float4 color;
    float2 texCoord;
    float  texIndex;
	float  tilingFactor;
};

struct InstanceData {
    float4x4 instanceTransform;
    float4 instanceColor;
};

struct VertexData {
	float4 position;
    float4 color;
    float2 texCoord;
    float  texIndex;
	float  tilingFactor;
};

struct GlobalUniforms {
    float4x4 viewProjection;
};

struct Push {
    float4x4 model;
};

v2f vertex vertexMain(uint vertexId                         [[vertex_id]],
                      device const VertexData* vertexData   [[buffer(0)]],
                      device const GlobalUniforms& uniform  [[buffer(1)]],
                      constant Push& push                   [[buffer(2)]]) {
    v2f o;
    float4 pos = float4(vertexData[vertexId].position);
    pos = uniform.viewProjection * push.model * pos;
    o.position = pos;
    o.color = float4(vertexData[vertexId].color);
    o.texCoord = vertexData[vertexId].texCoord;
	o.texIndex = vertexData[vertexId].texIndex;
	o.tilingFactor = vertexData[vertexId].tilingFactor;
    return o;
}

float4 fragment fragmentMain(v2f in [[stage_in]],
                        	array<texture2d<float, access::sample>, 32> tex [[texture(0)]]) {
    constexpr sampler s(address::repeat, filter::linear);
    float4 texel = tex[in.texIndex].sample(s, in.texCoord * in.tilingFactor).rgba;

    return float4(in.color * texel);
}
