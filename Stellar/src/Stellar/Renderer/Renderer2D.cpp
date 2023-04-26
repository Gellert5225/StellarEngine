#include "stlrpch.h"
#include "Renderer2D.h"

#include "Stellar/Renderer/Buffer.h"
#include "Stellar/Renderer/Shader.h"
#include "Stellar/Renderer/Uniforms.h"
#include "Stellar/Renderer/Renderer.h"
#include "Stellar/Renderer/Pipeline.h"
#include "Stellar/Renderer/Material.h"
#include "Stellar/Renderer/FrameBuffer.h"

namespace Stellar {
	struct QuadVertex {
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TexCoord;
		float TexIndex;
		float TilingFactor;
	};

	struct Renderer2DData {
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

		STLR_Ptr<Buffer> quadVertexBuffer;
		STLR_Ptr<Buffer> quadIndexBuffer;
		STLR_Ptr<Pipeline> quadPipeline;
		STLR_Ptr<Material> quadMaterial;

		uint32_t QuadIndexCount = 0;
		QuadVertex* QuadVertexBufferBase = nullptr;
		QuadVertex* QuadVertexBufferPtr = nullptr;
	};

	static Renderer2DData* s_Data;

	void Renderer2D::Init() {
		// FrameBufferSpec framebufferSpec;
		// framebufferSpec.attachments = { ImageFormat::RGBA32F, ImageFormat::Depth };

		// STLR_Ptr<FrameBuffer> framebuffer = FrameBuffer::Create(framebufferSpec);

		// RenderPassSpecification renderPassSpec;
		// renderPassSpec.targetFramebuffer = framebuffer;
		// renderPassSpec.debugName = "Renderer2D";
		// STLR_Ptr<RenderPass> renderPass = RenderPass::Create(renderPassSpec);

		// PipelineSpecification pipelineSpecification;
		// pipelineSpecification.debugName = "Renderer2D-Quad";
		// pipelineSpecification.shader = Renderer::GetShaderLibrary()->get("shader");
		// pipelineSpecification.renderPass = renderPass;
		// pipelineSpecification.backfaceCulling = false;
		// pipelineSpecification.layout = {
		// 	{ "a_Position", ShaderDataType::Float3 },
		// 	{ "a_Color", ShaderDataType::Float4 },
		// 	{ "a_TexCoord", ShaderDataType::Float2 },
		// 	{ "a_TexIndex", ShaderDataType::Float },
		// 	{ "a_TilingFactor", ShaderDataType::Float }
		// };
		// s_Data.quadPipeline = Pipeline::Create(pipelineSpecification);

		// s_Data.QuadVertexBufferBase = new QuadVertex[s_Data.MaxVertices];
		// s_Data.quadVertexBuffer = Buffer::Create(BufferType::Vertex, s_Data.MaxVertices * sizeof(QuadVertex), s_Data.QuadVertexBufferBase);

		// uint32_t* quadIndices = new uint32_t[s_Data.MaxIndices];

		// uint32_t offset = 0;
		// for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6) {
		// 	quadIndices[i + 0] = offset + 0;
		// 	quadIndices[i + 1] = offset + 1;
		// 	quadIndices[i + 2] = offset + 2;

		// 	quadIndices[i + 3] = offset + 2;
		// 	quadIndices[i + 4] = offset + 3;
		// 	quadIndices[i + 5] = offset + 0;

		// 	offset += 4;
		// }

		// s_Data.quadIndexBuffer = Buffer::Create(BufferType::Index, s_Data.MaxIndices, quadIndices);
		// delete[] quadIndices;

		s_Data = new Renderer2DData();

		const std::vector<Vertex> vertices = {
			{{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, 1.0f},
			{{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, 1.0f},
			{{ 1.0f,  1.0f, 0.0f}, {0.0f, 1.0f}, 1.0f},
			{{-1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}, 1.0f}
		};

		const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0
		};

		s_Data->QuadIndexCount = indices.size();

		// vertex buffer
		auto vertexBufferSize = sizeof(vertices[0]) * vertices.size();
		s_Data->quadVertexBuffer = Buffer::Create(BufferType::Vertex, vertexBufferSize, vertices.data());

		// index buffer
		auto indexBufferSize = sizeof(indices[0]) * indices.size();
		s_Data->quadIndexBuffer = Buffer::Create(BufferType::Index, indexBufferSize, indices.data());
	}

	void Renderer2D::ShutDown() {
		delete s_Data;
	}

	void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform) {
		GlobalUniforms ubo{};
		ubo.viewProjection = camera.getProjectionMatrix() * glm::inverse(transform);

		Renderer::BindUbo(ubo);
		Renderer::BeginRenderPass();

		Renderer::RenderGrid(s_Data->quadVertexBuffer, s_Data->quadIndexBuffer, s_Data->QuadIndexCount);
	}
	
	void Renderer2D::BeginScene(const EditorCamera& camera) {
		GlobalUniforms ubo{};
		ubo.viewProjection = camera.getViewProjectionMatrix();

		Renderer::BindUbo(ubo);
		Renderer::BeginRenderPass();

		Renderer::RenderGrid(s_Data->quadVertexBuffer, s_Data->quadIndexBuffer, s_Data->QuadIndexCount);
	}

	void Renderer2D::EndScene() {
		Renderer::EndRenderPass();
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color) {
		//Renderer::RenderGeometry(s_Data->quadVertexBuffer, s_Data->quadIndexBuffer, color, s_Data->indexCount);
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform, const glm::vec4& color, STLR_Ptr<Texture2D> texture) {
		Renderer::RenderGeometry(s_Data->quadVertexBuffer, s_Data->quadIndexBuffer, texture, color, s_Data->QuadIndexCount, transform);
	}
} 