#pragma once

#include "Stellar/Core/Core.h"

#include "Stellar/Renderer/RendererAPI.h"
#include "Stellar/Renderer/Buffer.h"

#include "Stellar/Platform/Vulkan/SwapChain/VulkanSwapChain.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanFrameBuffer.h"
#include "Stellar/Core/Application.h"

#include <vulkan/vulkan.h>

namespace Stellar {
	class STLR_API VulkanRenderer : public RendererAPI {
	public:
		void init() override;
		void shutDown() override;

		void beginRenderPass() override;
		void endRenderPass() override;

		void setClearColor(const glm::vec4& color) override;
		void renderGeometry(Buffer* vertexBuffer,
							Buffer* indexBuffer,
							Ref<Texture2D> texture,
							const glm::vec4& color,
							uint32_t indexCount,
							const glm::mat4& transform) override;
		void renderGrid(Buffer* vertexBuffer,
						Buffer* indexBuffer,
						uint32_t indexCount = 0) override;

		Ref<FrameBuffer> getFrameBuffer() override;
		void resizeFrameBuffer(uint32_t width, uint32_t height) override;
		
		void bindUbo(const GlobalUniforms& ubo) override;

		// vulkan
		static VulkanPipeline* GetPipeline();
		static VkDescriptorPool GetDescriptorPool();
		static VkDescriptorSet AllocateDesriptorSet(VkDescriptorSetAllocateInfo& allocInfo);
	private:
		VulkanPipeline* m_GraphicsPipeline = nullptr;
		VulkanPipeline* m_GridPipeline = nullptr;
		VkClearColorValue m_ClearColor = {{0.66f, 0.9f, 0.96f, 1.0f}};

		Buffer* m_UniformBuffer{};
		Ref<FrameBuffer> m_FrameBuffer;

		VkDescriptorSet m_UboDescriptorSet;

		bool m_NeedResize = false;
		uint32_t m_ViewPortWidth, m_ViewPortHeight;

		void createUboDescriptorSet();
	};
}