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
			{ "inColor", ShaderDataType::Float4 },
			{ "inTexCoord", ShaderDataType::Float2 },
			{ "inTexIndex", ShaderDataType::Float },
			{ "inTilingFactor", ShaderDataType::Float }
		};
		m_QuadPipeline = Pipeline::Create(pipelineSpecification);

		m_QuadVertexPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[1] = { -0.5f,  0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		m_QuadVertexPositions[3] = {  0.5f, -0.5f, 0.0f, 1.0f };

		m_QuadVertexBufferBase = new QuadVertex[MaxVertices];
		m_QuadVertexBuffer = Buffer::Create(BufferType::Vertex, MaxVertices * sizeof(QuadVertex));

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

		m_WhiteTexture = Texture2D::Create(ImageFormat::RGBA, 1, 1);
		m_TextureSlots[0] = m_WhiteTexture;

		m_RenderCommandBuffer = CommandBuffer::Create(Renderer::MAX_FRAMES_IN_FLIGHT);

		m_UniformBufferSet = UniformBufferSet::Create(Renderer::MAX_FRAMES_IN_FLIGHT);
		m_UniformBufferSet->create(sizeof(GlobalUniforms), 0);

		m_QuadMaterial = Material::Create(m_QuadPipeline->getSpecification().shader, "QuadMaterial");
	}

	void Renderer2D::shutDown() {
	}

	void Renderer2D::beginScene(const Camera& camera, const glm::mat4& transform) {
		auto viewProjection = camera.getProjectionMatrix() * glm::inverse(transform);

		uint32_t bufferIndex = Renderer::GetCurrentFrameIndex();
		m_UniformBufferSet->get(0, 0, bufferIndex)->setData(&viewProjection, sizeof(GlobalUniforms));

		m_QuadIndexCount = 0;
		m_QuadVertexBufferPtr = m_QuadVertexBufferBase;
		m_TextureSlotIndex = 1;

		//Renderer::BindUbo(ubo);
		//Renderer::BeginRenderPass();

		//Renderer::RenderGrid(m_QuadVertexBuffer, m_QuadIndexBuffer, m_QuadIndexCount);
	}
	
	void Renderer2D::beginScene(const EditorCamera& camera) {
		auto viewProjection = camera.getViewProjectionMatrix();

		uint32_t bufferIndex = Renderer::GetCurrentFrameIndex();
		m_UniformBufferSet->get(0, 0, bufferIndex)->setData(&viewProjection, sizeof(GlobalUniforms));

		m_QuadIndexCount = 0;
		m_QuadVertexBufferPtr = m_QuadVertexBufferBase;
		m_TextureSlotIndex = 1;

		for (uint32_t i = 1; i < m_TextureSlots.size(); i++)
			m_TextureSlots[i] = nullptr;

		//Renderer::RenderGrid(m_QuadVertexBuffer, m_QuadIndexBuffer, m_QuadIndexCount);
	}

	void Renderer2D::endScene() {
		m_RenderCommandBuffer->begin();
		Renderer::BeginRenderPass(m_RenderCommandBuffer, m_QuadPipeline->getSpecification().renderPass);

		uint32_t dataSize = (uint8_t*)m_QuadVertexBufferPtr - (uint8_t*)m_QuadVertexBufferBase;
		if (dataSize) {
			m_QuadVertexBuffer->setData(m_QuadVertexBufferBase, dataSize);

			for (uint32_t i = 0; i < m_TextureSlots.size(); i++) {
				if (m_TextureSlots[i])
					m_QuadMaterial->set("texSampler", m_TextureSlots[i]);
				else
					m_QuadMaterial->set("texSampler", m_WhiteTexture);
			}


			Renderer::RenderGeometry(m_RenderCommandBuffer, m_QuadPipeline, m_UniformBufferSet, m_QuadMaterial, m_QuadVertexBuffer, m_QuadIndexBuffer, glm::mat4(1.0f), m_QuadIndexCount);

			//s_Data.Stats.DrawCalls++;
		}

		Renderer::EndRenderPass(m_RenderCommandBuffer);
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color, STLR_Ptr<Texture2D> texture) {
		//Renderer::RenderGeometry(m_QuadVertexBuffer, m_QuadIndexBuffer, texture, color, m_QuadIndexCount, transform);
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

		m_QuadIndexCount = 0;
		m_QuadVertexBufferPtr = m_QuadVertexBufferBase;

		m_TextureSlotIndex = 1;
	}
} 