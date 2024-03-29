#pragma once

#include "Stellar/Core/Core.h"

#include "Stellar/Renderer/RendererAPI.h"
#include "Stellar/Renderer/Buffer.h"
#include "Stellar/Renderer/UniformBuffer.h"

#include "Stellar/Platform/Vulkan/SwapChain/VulkanSwapChain.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanFrameBuffer.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanBuffer.h"
#include "Stellar/Core/Application.h"

#include <vulkan/vulkan.h>

namespace Stellar {
	class VulkanRenderer : public RendererAPI {
	public:
		void init() override;
		void shutDown() override;

		void beginRenderPass(STLR_Ptr<CommandBuffer> commandBuffer, 
							STLR_Ptr<RenderPass> renderPass, 
							bool explicitClear = false) override;
		void endRenderPass(STLR_Ptr<CommandBuffer> commandBuffer) override;

		void beginFrame() override;

		void setClearColor(const glm::vec4& color) override;
		void renderGeometry(STLR_Ptr<CommandBuffer> renderCommandBuffer, 
							STLR_Ptr<Pipeline> pipeline,
							STLR_Ptr<UniformBufferSet> uniformBufferSet, 
							STLR_Ptr<Material> material, 
							STLR_Ptr<Buffer> vertexBuffer, 
							STLR_Ptr<Buffer> indexBuffer, 
							const glm::mat4& transform, 
							uint32_t indexCount = 0) override;
		void renderGrid(STLR_Ptr<Buffer> vertexBuffer,
						STLR_Ptr<Buffer> indexBuffer,
						uint32_t indexCount = 0) override;

		STLR_Ptr<FrameBuffer> getFrameBuffer() override;
		void resizeFrameBuffer(uint32_t width, uint32_t height) override;
		
		void bindUbo(const GlobalUniforms& ubo) override;

		// vulkan
		static VkDescriptorPool GetDescriptorPool();
		static VkDescriptorSet AllocateDesriptorSet(VkDescriptorSetAllocateInfo& allocInfo);
	private:
		VkClearColorValue m_ClearColor = {{0.66f, 0.9f, 0.96f, 1.0f}};

		STLR_Ptr<FrameBuffer> m_FrameBuffer;

		VkDescriptorSet m_UboDescriptorSet;

		bool m_NeedResize = false;
		uint32_t m_ViewPortWidth, m_ViewPortHeight;
	};
}