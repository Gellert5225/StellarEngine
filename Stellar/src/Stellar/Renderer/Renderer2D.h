#pragma once

#include "Stellar/Renderer/Camera.h"
#include "Stellar/Renderer/Texture.h"
#include "Stellar/Renderer/Buffer.h"
#include "Stellar/Renderer/Shader.h"
#include "Stellar/Renderer/Uniforms.h"
#include "Stellar/Renderer/Renderer.h"
#include "Stellar/Renderer/Pipeline.h"
#include "Stellar/Renderer/Material.h"
#include "Stellar/Renderer/FrameBuffer.h"

#include "Stellar/Core/Core.h"
#include "Stellar/Core/STLRBase.h"
#include "Stellar/Editor/EditorCamera.h"

namespace Stellar {
	class Renderer2D : public STLR_Base {
	public:
		struct Statistics {
			uint32_t drawCalls = 0;
			uint32_t quadCount = 0;
			uint32_t lineCount = 0;

			uint32_t getTotalVertexCount() { return quadCount * 4 + lineCount * 2; }
			uint32_t getTotalIndexCount() { return quadCount * 6 + lineCount * 2; }
		};
		void resetStats();
		Statistics getStats();

	public:
		Renderer2D();
		virtual ~Renderer2D() {}

		void init();
		void shutDown();

		void beginScene(const EditorCamera& camera);
		void beginScene(const Camera& camera, const glm::mat4& transform);
		void endScene();
		void drawQuad(const glm::mat4& transform, const glm::vec4& color);
		void drawQuad(const glm::mat4& transform, const glm::vec4& color, const STLR_Ptr<Texture2D>& texture, float tilingFactor);
	private:
		void flushAndReset();
	private:
		struct QuadVertex {
			glm::vec4 Position;
			glm::vec4 Color;
			glm::vec2 TexCoord;
			float TexIndex;
			float TilingFactor;
		};
		static const uint32_t MaxQuads = 20000;
		static const uint32_t MaxVertices = MaxQuads * 4;
		static const uint32_t MaxIndices = MaxQuads * 6;
		static const uint32_t MaxTextureSlots = 32; // TODO: RenderCaps

		std::vector<STLR_Ptr<Buffer>> m_QuadVertexBuffer;
		STLR_Ptr<Buffer> m_QuadIndexBuffer;
		STLR_Ptr<Pipeline> m_QuadPipeline;
		STLR_Ptr<Material> m_QuadMaterial;
		STLR_Ptr<Texture2D> m_WhiteTexture;
		STLR_Ptr<CommandBuffer> m_RenderCommandBuffer;

		uint32_t m_QuadIndexCount = 0;
		std::vector<QuadVertex*> m_QuadVertexBufferBase;
		QuadVertex* m_QuadVertexBufferPtr = nullptr;

		std::array<STLR_Ptr<Texture2D>, MaxTextureSlots> m_TextureSlots;
		uint32_t m_TextureSlotIndex = 1; // 0 = white texture

		glm::vec4 m_QuadVertexPositions[4];

		STLR_Ptr<UniformBufferSet> m_UniformBufferSet;

		Statistics m_Stats;
	};
}