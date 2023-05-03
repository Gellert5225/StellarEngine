#include "stlrpch.h"
#include "Renderer2D.h"

namespace Stellar {
	Renderer2D::Renderer2D() {
		init();
	}

	void Renderer2D::init() {
		FrameBufferSpec framebufferSpec;
		framebufferSpec.width = 1280;
		framebufferSpec.height = 720;
		framebufferSpec.attachments = { ImageFormat::RGBA32F, ImageFormat::Depth };

		STLR_Ptr<FrameBuffer> framebuffer = FrameBuffer::Create(framebufferSpec);

		RenderPassSpecification renderPassSpec;
		renderPassSpec.targetFramebuffer = framebuffer;
		renderPassSpec.debugName = "Renderer2D";
		STLR_Ptr<RenderPass> renderPass = RenderPass::Create(renderPassSpec);

		PipelineSpecification pipelineSpecification;
		pipelineSpecification.debugName = "Renderer2D-Quad";
		pipelineSpecification.shader = Renderer::GetShaderLibrary()->get("shader");
		pipelineSpecification.renderPass = renderPass;
		pipelineSpecification.backfaceCulling = false;
		pipelineSpecification.layout = {
			{ "inPosition", ShaderDataType::Float3 },
			{ "inTexCoord", ShaderDataType::Float2 },
			{ "inTilingFactor", ShaderDataType::Float }
		};
		m_QuadPipeline = Pipeline::Create(pipelineSpecification);

		m_QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[1] = { -0.5f,  0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[3] = {  0.5f, -0.5f, 0.0f, 1.0f };

		m_QuadVertexBufferBase = new QuadVertex[MaxVertices];
		m_QuadVertexBuffer = Buffer::Create(BufferType::Vertex, MaxVertices * sizeof(QuadVertex), m_QuadVertexBufferBase);

		uint32_t* quadIndices = new uint32_t[MaxIndices];

		uint32_t offset = 0;
		for (uint32_t i = 0; i < MaxIndices; i += 6) {
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		m_QuadIndexBuffer = Buffer::Create(BufferType::Index, MaxIndices, quadIndices);
		delete[] quadIndices;

		const std::vector<Vertex> vertices = {
			{{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}, 1.0f},
			{{ 1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}, 1.0f},
			{{ 1.0f,  1.0f, 0.0f}, {0.0f, 1.0f}, 1.0f},
			{{-1.0f,  1.0f, 0.0f}, {1.0f, 1.0f}, 1.0f}
		};

		const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0
		};

		m_QuadIndexCount = indices.size();

		// vertex buffer
		auto vertexBufferSize = sizeof(vertices[0]) * vertices.size();
		m_QuadVertexBuffer = Buffer::Create(BufferType::Vertex, vertexBufferSize, vertices.data());

		// index buffer
		auto indexBufferSize = sizeof(indices[0]) * indices.size();
		m_QuadIndexBuffer = Buffer::Create(BufferType::Index, indexBufferSize, indices.data());
	}

	void Renderer2D::shutDown() {
	}

	void Renderer2D::beginScene(const Camera& camera, const glm::mat4& transform) {
		GlobalUniforms ubo{};
		ubo.viewProjection = camera.getProjectionMatrix() * glm::inverse(transform);

		Renderer::BindUbo(ubo);
		Renderer::BeginRenderPass();

		Renderer::RenderGrid(m_QuadVertexBuffer, m_QuadIndexBuffer, m_QuadIndexCount);
	}
	
	void Renderer2D::beginScene(const EditorCamera& camera) {
		GlobalUniforms ubo{};
		ubo.viewProjection = camera.getViewProjectionMatrix();

		Renderer::BindUbo(ubo);
		Renderer::BeginRenderPass();

		Renderer::RenderGrid(m_QuadVertexBuffer, m_QuadIndexBuffer, m_QuadIndexCount);
	}

	void Renderer2D::endScene() {
		Renderer::EndRenderPass();
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color, STLR_Ptr<Texture2D> texture) {
		Renderer::RenderGeometry(m_QuadVertexBuffer, m_QuadIndexBuffer, texture, color, m_QuadIndexCount, transform);
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color, const STLR_Ptr<Texture2D>& texture, float tilingFactor) {
		constexpr size_t quadVertexCount = 4;
		//constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };

		if (m_QuadIndexCount >= MaxIndices)
			flushAndReset();

		float textureIndex = 0.0f;
		for (uint32_t i = 1; i < m_TextureSlotIndex; i++) {
			if (*m_TextureSlots[i].raw() == *texture.raw()) {
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f) {
			if (m_TextureSlotIndex >= MaxTextureSlots)
				flushAndReset();

			textureIndex = (float)m_TextureSlotIndex;
			m_TextureSlots[m_TextureSlotIndex] = texture;
			m_TextureSlotIndex++;
		}

		for (size_t i = 0; i < quadVertexCount; i++) {
			m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[i];
			m_QuadVertexBufferPtr->Color = color;
			m_QuadVertexBufferPtr->TexCoord = textureCoords[i];
			m_QuadVertexBufferPtr->TexIndex = textureIndex;
			m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
			m_QuadVertexBufferPtr++;
		}

		m_QuadIndexCount += 6;

		//s_Data.Stats.QuadCount++;
	}

	void Renderer2D::flushAndReset() {
		endScene();

		// s_Data.quadIndexCount = 0;
		// s_Data.quadVertexBufferPtr = s_Data.quadVertexBufferBase;

		// s_Data.textureSlotIndex = 1;
	}
} 