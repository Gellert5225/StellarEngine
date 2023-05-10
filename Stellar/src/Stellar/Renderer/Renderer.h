#pragma once

#include "RendererAPI.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#include "Camera.h"
#include "Shader.h"
#include "Uniforms.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "Pipeline.h"
#include "Material.h"
#include "UniformBufferSet.h"

#include "Stellar/Core/Core.h"

#include <glm/glm.hpp>

namespace Stellar {

	class Renderer {
	public:
		static void Init();
		static void Shutdown();

		static void BeginRenderPass(STLR_Ptr<CommandBuffer> commandBuffer, 
									STLR_Ptr<RenderPass> renderPass, 
									bool explicitClear = false);
		static void EndRenderPass(STLR_Ptr<CommandBuffer> commandBuffer);

		static void BeginFrame();

		static void SetClearColor(const glm::vec4& color);
		static void RenderGeometry(STLR_Ptr<Buffer> vertexBuffer,
								STLR_Ptr<Buffer> indexBuffer,
								STLR_Ptr<Texture2D>,
								const glm::vec4& color,
								uint32_t indexCount = 0,
								const glm::mat4& transform = {});
		static void RenderGeometry(STLR_Ptr<CommandBuffer> commandBuffer, 
									STLR_Ptr<Pipeline> pipeline, 
									STLR_Ptr<UniformBufferSet> uniformBufferSet, 
									STLR_Ptr<Material> material, 
									STLR_Ptr<Buffer> vertexBuffer, 
									STLR_Ptr<Buffer> indexBuffer, 
									const glm::mat4& transform, 
									uint32_t indexCount = 0);
		static void RenderGrid(STLR_Ptr<Buffer> vertexBuffer,
							STLR_Ptr<Buffer> indexBuffer,
							uint32_t indexCount = 0);

		static void BindUbo(const GlobalUniforms& ubo);

		static STLR_Ptr<FrameBuffer> GetFrameBuffer();

		static void ResizeFrameBuffer(uint32_t width, uint32_t height);

		static ShaderLibrary* GetShaderLibrary();
		
		static uint32_t GetCurrentFrameIndex();

		static constexpr int MAX_FRAMES_IN_FLIGHT = 3;
	};
}