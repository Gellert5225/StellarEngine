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

#include "Stellar/Core/Core.h"

#include <glm/glm.hpp>

namespace Stellar {

	class STLR_API Renderer {
	public:
		static void Init();
		static void Shutdown();

		static void BeginRenderPass();
		static void EndRenderPass();

		static void SetClearColor(const glm::vec4& color);
		static void RenderGeometry(Buffer* vertexBuffer,
								Buffer* indexBuffer,
								Ref<Texture2D>,
								const glm::vec4& color,
								uint32_t indexCount = 0,
								const glm::mat4& transform = {});
		// static void RenderGeometry(Ref<CommandBuffer> commandBuffer, 
		// 							Ref<Pipeline> pipeline, 
		// 							Ref<UniformBufferSet> uniformBufferSet, 
		// 							Ref<Material> material, 
		// 							Ref<VertexBuffer> vertexBuffer, 
		// 							Ref<IndexBuffer> indexBuffer, 
		// 							const glm::mat4& transform, 
		// 							uint32_t indexCount = 0);
		static void RenderGrid(Buffer* vertexBuffer,
							Buffer* indexBuffer,
							uint32_t indexCount = 0);

		static void BindUbo(const GlobalUniforms& ubo);

		static Ref<FrameBuffer> GetFrameBuffer();

		static void ResizeFrameBuffer(uint32_t width, uint32_t height);

		static ShaderLibrary* GetShaderLibrary();
		
		static uint32_t GetCurrentFrameIndex();

		static constexpr int MAX_FRAMES_IN_FLIGHT = 3;
	};
}