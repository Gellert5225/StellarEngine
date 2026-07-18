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
			{ "inPosition", 	ShaderDataType::Float4 },
			{ "inColor", 		ShaderDataType::Float4 },
			{ "inTexCoord", 	ShaderDataType::Float2 },
			{ "inTexIndex", 	ShaderDataType::Float },
			{ "inTilingFactor", ShaderDataType::Float }
		};
		m_QuadPipeline = Pipeline::Create(pipelineSpecification);

		auto framesInFlight = Renderer::MAX_FRAMES_IN_FLIGHT;

		m_QuadVertexBuffer.resize(framesInFlight);
		m_QuadVertexBufferBase.resize(framesInFlight);
		for (uint32_t i = 0; i < framesInFlight; i++) {
			m_QuadVertexBuffer[i] = Buffer::Create(BufferType::Vertex, MaxVertices * sizeof(QuadVertex));
			m_QuadVertexBufferBase[i] = new QuadVertex[MaxVertices];
		}

		m_QuadVertexPositions[0] = { -1.0f, -1.0f, 0.0f, 1.0f };
		m_QuadVertexPositions[1] = {  1.0f, -1.0f, 0.0f, 1.0f };
		m_QuadVertexPositions[2] = {  1.0f,  1.0f, 0.0f, 1.0f };
		m_QuadVertexPositions[3] = { -1.0f,  1.0f, 0.0f, 1.0f };

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

		m_WhiteTexture = Texture2D::Create({});
		m_TextureSlots[0] = m_WhiteTexture;

		m_RenderCommandBuffer = CommandBuffer::Create(Renderer::MAX_FRAMES_IN_FLIGHT);

		m_UniformBufferSet = UniformBufferSet::Create(Renderer::MAX_FRAMES_IN_FLIGHT);
		m_UniformBufferSet->create(sizeof(GlobalUniforms), 0);

		m_QuadMaterial = Material::Create(m_QuadPipeline->getSpecification().shader, "QuadMaterial");

        PipelineSpecification meshSpec;
        meshSpec.shader = Renderer::GetShaderLibrary()->get("mesh");
        meshSpec.renderPass = renderPass;
        meshSpec.layout = { 
            {"inPosition", ShaderDataType::Float3},
            {"inNormal",   ShaderDataType::Float3},
            {"inUV",       ShaderDataType::Float2} 
        };
        meshSpec.backfaceCulling = false;   // first cut: don't cull, so winding / the flipped viewport can't hide faces
        m_MeshPipeline = Pipeline::Create(meshSpec);
        m_MeshMaterial = Material::Create(meshSpec.shader, "MeshMaterial");

        // Unit cube centered at origin, per-face normals (flat shading).
        MeshVertex cubeVertices[24] = {
            // Front (+Z)
            {{-0.5f,-0.5f, 0.5f}, { 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
            {{ 0.5f,-0.5f, 0.5f}, { 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
            {{ 0.5f, 0.5f, 0.5f}, { 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f, 0.5f}, { 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
            // Back (-Z)
            {{ 0.5f,-0.5f,-0.5f}, { 0.0f, 0.0f,-1.0f}, {0.0f, 0.0f}},
            {{-0.5f,-0.5f,-0.5f}, { 0.0f, 0.0f,-1.0f}, {1.0f, 0.0f}},
            {{-0.5f, 0.5f,-0.5f}, { 0.0f, 0.0f,-1.0f}, {1.0f, 1.0f}},
            {{ 0.5f, 0.5f,-0.5f}, { 0.0f, 0.0f,-1.0f}, {0.0f, 1.0f}},
            // Left (-X)
            {{-0.5f,-0.5f,-0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{-0.5f,-0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f,-0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
            // Right (+X)
            {{ 0.5f,-0.5f, 0.5f}, { 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f,-0.5f,-0.5f}, { 1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
            {{ 0.5f, 0.5f,-0.5f}, { 1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
            {{ 0.5f, 0.5f, 0.5f}, { 1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
            // Top (+Y)
            {{-0.5f, 0.5f, 0.5f}, { 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f, 0.5f, 0.5f}, { 0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
            {{ 0.5f, 0.5f,-0.5f}, { 0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f, 0.5f,-0.5f}, { 0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
            // Bottom (-Y)
            {{-0.5f,-0.5f,-0.5f}, { 0.0f,-1.0f, 0.0f}, {0.0f, 0.0f}},
            {{ 0.5f,-0.5f,-0.5f}, { 0.0f,-1.0f, 0.0f}, {1.0f, 0.0f}},
            {{ 0.5f,-0.5f, 0.5f}, { 0.0f,-1.0f, 0.0f}, {1.0f, 1.0f}},
            {{-0.5f,-0.5f, 0.5f}, { 0.0f,-1.0f, 0.0f}, {0.0f, 1.0f}},
        };
        uint32_t cubeIndices[36] = {
             0, 1, 2,  2, 3, 0,   // front
             4, 5, 6,  6, 7, 4,   // back
             8, 9,10, 10,11, 8,   // left
            12,13,14, 14,15,12,   // right
            16,17,18, 18,19,16,   // top
            20,21,22, 22,23,20,   // bottom
        };
        m_CubeVertexBuffer = Buffer::Create(BufferType::Vertex, sizeof(cubeVertices), cubeVertices);
        m_CubeIndexBuffer  = Buffer::Create(BufferType::Index,  sizeof(cubeIndices),  cubeIndices);
	}

	void Renderer2D::shutDown() {
		for (auto i : m_QuadVertexBufferBase) {
			delete i;
		}
	}

	void Renderer2D::beginScene(const Camera& camera, const glm::mat4& transform) {
		auto proje = camera.getProjectionMatrix();
		auto viewProjection = camera.getProjectionMatrix() * glm::inverse(transform);

		uint32_t bufferIndex = Renderer::GetCurrentFrameIndex();
		m_UniformBufferSet->get(0, 0, bufferIndex)->setData(&viewProjection, sizeof(GlobalUniforms));
		
		m_QuadIndexCount = 0;
		m_QuadVertexBufferPtr = m_QuadVertexBufferBase[bufferIndex];
		m_TextureSlotIndex = 1;
		m_CubeTransforms.clear();

		for (uint32_t i = 1; i < m_TextureSlots.size(); i++)
			m_TextureSlots[i] = nullptr;

		//Renderer::RenderGrid(m_QuadVertexBuffer, m_QuadIndexBuffer, m_QuadIndexCount);
	}
	
	void Renderer2D::beginScene(const EditorCamera& camera) {
		auto viewProjection = camera.getViewProjectionMatrix();

		uint32_t bufferIndex = Renderer::GetCurrentFrameIndex();
		m_UniformBufferSet->get(0, 0, bufferIndex)->setData(&viewProjection, sizeof(GlobalUniforms));

		m_QuadIndexCount = 0;
		m_QuadVertexBufferPtr = m_QuadVertexBufferBase[bufferIndex];
		m_TextureSlotIndex = 1;
		m_CubeTransforms.clear();

		for (uint32_t i = 1; i < m_TextureSlots.size(); i++)
			m_TextureSlots[i] = nullptr;

		//Renderer::RenderGrid(m_QuadVertexBuffer, m_QuadIndexBuffer, m_QuadIndexCount);
	}

	void Renderer2D::endScene() {
		m_RenderCommandBuffer->begin();
		Renderer::BeginRenderPass(m_RenderCommandBuffer, m_QuadPipeline->getSpecification().renderPass);
		uint32_t frameIndex = Renderer::GetCurrentFrameIndex();

		// Meshes draw into the same render pass, so they composite with the 2D layer.
		for (const auto& transform : m_CubeTransforms) {
			Renderer::RenderGeometry(m_RenderCommandBuffer, m_MeshPipeline, m_UniformBufferSet,
				m_MeshMaterial, m_CubeVertexBuffer, m_CubeIndexBuffer, transform, 36);
			m_Stats.drawCalls++;
		}

		uint32_t dataSize = (uint8_t*)m_QuadVertexBufferPtr - (uint8_t*)m_QuadVertexBufferBase[frameIndex];
		if (dataSize) {
			m_QuadVertexBuffer[frameIndex]->setData(m_QuadVertexBufferBase[frameIndex], dataSize);

			for (uint32_t i = 0; i < m_TextureSlots.size(); i++) {
				if (m_TextureSlots[i])
					m_QuadMaterial->set("texSampler", m_TextureSlots[i], i);
				else
					m_QuadMaterial->set("texSampler", m_WhiteTexture, i);
			}

			Renderer::RenderGeometry(m_RenderCommandBuffer, 
                m_QuadPipeline, 
                m_UniformBufferSet, 
                m_QuadMaterial, 
                m_QuadVertexBuffer[frameIndex], 
                m_QuadIndexBuffer, 
                glm::mat4(1.0f), 
                m_QuadIndexCount
            );

			m_Stats.drawCalls++;
		}

		Renderer::EndRenderPass(m_RenderCommandBuffer);
		m_RenderCommandBuffer->end();
		m_RenderCommandBuffer->submit();
	}

	void Renderer2D::drawCube(const glm::mat4& transform) {
		m_CubeTransforms.push_back(transform);
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color) {
		constexpr size_t quadVertexCount = 4;
		const float textureIndex = 0.0f; // White Texture
		constexpr glm::vec2 textureCoords[] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f } };
		const float tilingFactor = 1.0f;

		if (m_QuadIndexCount >= MaxIndices)
			flushAndReset();

		for (size_t i = 0; i < quadVertexCount; i++) {
			m_QuadVertexBufferPtr->Position = transform * m_QuadVertexPositions[i];
			m_QuadVertexBufferPtr->Color = color;
			m_QuadVertexBufferPtr->TexCoord = textureCoords[i];
			m_QuadVertexBufferPtr->TexIndex = textureIndex;
			m_QuadVertexBufferPtr->TilingFactor = tilingFactor;
			m_QuadVertexBufferPtr++;
		}

		m_QuadIndexCount += 6;
		m_Stats.quadCount++;
	}
	
	// Draws a grid on the X-Z plane centered at the origin
	void Renderer2D::drawGrid(int gridCount, float spacing, const glm::vec4& color) {
	    // Grid lines are drawn as thin quads for batch compatibility
		float gridLength = gridCount * spacing;
		float lineThickness = spacing * 0.005f;

		glm::vec4 xAxisColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
		glm::vec4 zAxisColor = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue

		for (int i = -gridCount; i <= gridCount; ++i) {
			float pos = i * spacing;
			// X lines (parallel to X, at constant Z): color X axis red
			glm::vec4 xLineColor = (i == 0) ? xAxisColor : color;
			glm::mat4 xLineTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, pos))
				* glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0))
				* glm::scale(glm::mat4(1.0f), glm::vec3(gridLength * 2.0f, lineThickness, lineThickness));
			drawQuad(xLineTransform, xLineColor);
			// Z lines (parallel to Z, at constant X): color Z axis blue
			glm::vec4 zLineColor = (i == 0) ? zAxisColor : color;
			glm::mat4 zLineTransform = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f, 0.0f))
                * glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0))
				* glm::scale(glm::mat4(1.0f), glm::vec3(lineThickness, gridLength * 2.0f, lineThickness));
			drawQuad(zLineTransform, zLineColor);
		}
	}

	void Renderer2D::drawQuad(const glm::mat4& transform, const glm::vec4& color, const STLR_Ptr<Texture2D>& texture, float tilingFactor) {
		constexpr size_t quadVertexCount = 4;
		//constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		constexpr glm::vec2 textureCoords[] = { { 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } };

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

		m_Stats.quadCount++;
	}

	void Renderer2D::flushAndReset() {
		uint32_t frameIndex = Renderer::GetCurrentFrameIndex();
		//EndScene();

		m_QuadIndexCount = 0;
		m_QuadVertexBufferPtr = m_QuadVertexBufferBase[frameIndex];

		m_TextureSlotIndex = 1;
	}

	void Renderer2D::resetStats() {
		memset(&m_Stats, 0, sizeof(Statistics));
	}

	Renderer2D::Statistics Renderer2D::getStats() {
		return m_Stats;
	}
} 