#pragma once

#include "Buffer.h"
#include "CommandBuffer.h"
#include "Camera.h"
#include "Uniforms.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "Pipeline.h"
#include "Material.h"
#include "UniformBufferSet.h"

#include <glm/glm.hpp>

namespace Stellar {
	enum class RendererAPIType {
		None = 0, Vulkan = 1, Metal = 2
	};

	class RendererAPI {
	public:
		virtual ~RendererAPI() = default;
		virtual void init() = 0;
		virtual void shutDown() = 0;

		virtual void beginRenderPass(STLR_Ptr<CommandBuffer> commandBuffer, 
									STLR_Ptr<RenderPass> renderPass, 
									bool explicitClear = false) = 0;
		virtual void endRenderPass(STLR_Ptr<CommandBuffer> commandBuffer) = 0;

		virtual void beginFrame() = 0;

		virtual void setClearColor(const glm::vec4& color) = 0;

		virtual void renderGrid(STLR_Ptr<Buffer> vertexBuffer,
								STLR_Ptr<Buffer> indexBuffer,
								uint32_t indexCount = 0) = 0;

		virtual void renderGeometry(STLR_Ptr<CommandBuffer> renderCommandBuffer, 
									STLR_Ptr<Pipeline> pipeline,
									STLR_Ptr<UniformBufferSet> uniformBufferSet, 
									STLR_Ptr<Material> material, 
									STLR_Ptr<Buffer> vertexBuffer, 
									STLR_Ptr<Buffer> indexBuffer, 
									const glm::mat4& transform, 
									uint32_t indexCount = 0) = 0;
		
		virtual void bindUbo(const GlobalUniforms& ubo) = 0;
		virtual STLR_Ptr<FrameBuffer> getFrameBuffer() = 0;
		virtual void resizeFrameBuffer(uint32_t width, uint32_t height) = 0;

		static RendererAPIType Current() { return s_CurrentRendererAPI; }
	protected:
		STLR_Ptr<CommandBuffer> m_CommandBuffer{};
		Camera m_Camera;
	private:
#ifdef __APPLE__
#define STLR_RENDERER_API RendererAPIType::Metal
#else
#define STLR_RENDERER_API RendererAPIType::Vulkan
#endif
		inline static RendererAPIType s_CurrentRendererAPI = STLR_RENDERER_API;

	};
}