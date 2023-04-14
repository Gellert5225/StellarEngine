#pragma once

#include "CommandPool.h"

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/CommandBuffer.h"
#include "Stellar/Platform/Vulkan/Pipeline/GraphicsPipeline.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanBuffer.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanFrameBuffer.h"

#include <vulkan/vulkan.h>

namespace Stellar {
	class STLR_API VulkanCommandBuffer : public CommandBuffer {
	public:
		explicit VulkanCommandBuffer(uint32_t size);
		VulkanCommandBuffer(uint32_t size, VkCommandBufferLevel level);
		~VulkanCommandBuffer() override;

		void begin() override;
		void end() override;
		void submit() override;

		void* getActiveCommandBuffer() override;

		[[nodiscard]] VkCommandBuffer getCurrentCommandBuffer(uint32_t currentFrameIndex) const;

		[[nodiscard]] const std::vector<VkCommandBuffer>* getCommandBuffers() const;
	private:
		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::vector<VkFence> m_WaitFences;
	};
}