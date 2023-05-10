#version 450 core
#pragma vert

layout(location = 0) in vec4 inPosition;
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

struct VertexOutput {
	vec4 color;
	vec2 texCoord;
	float texIndex;
	float tilingFactor;
};

layout (location = 0) out VertexOutput Output;

void main() {
    gl_Position = ubo.viewProjection * push.model * inPosition;
    Output.color = inColor;
	Output.texCoord = inTexCoord;
	Output.texIndex = inTexIndex;
	Output.tilingFactor = inTilingFactor;
}

#version 450 core
#pragma frag

layout(location = 0) out vec4 outColor;

struct VertexOutput {
	vec4 color;
	vec2 texCoord;
	float texIndex;
	float tilingFactor;
};

layout(location = 0) in VertexOutput Input;

layout(set = 0, binding = 1) uniform sampler2D texSampler[32];

void main() {
	//outColor = Input.color;
    outColor = texture(texSampler[int(Input.texIndex)], Input.texCoord * Input.tilingFactor) * Input.color;
}