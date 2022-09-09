#pragma once

#include "Stellar/Core.h"
#include "CommandPool.h"
#include "Stellar/Platform/Vulkan/RenderPass/StandardRenderPass.h"
#include "Stellar/Platform/Vulkan/Pipeline/GraphicsPipeline.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanBuffer.h"
#include "Stellar/Platform/Vulkan/Buffer/FrameBuffer.h"
#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API CommandBuffer {
    public:
        CommandBuffer(VkCommandPool commandPool, uint32_t size);
        CommandBuffer(VkCommandPool commandPool, uint32_t size, VkCommandBufferLevel level);
        ~CommandBuffer();

        void endRenderPass() const;
        void endCommandBuffer() const;

        [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer(uint32_t currentFrameIndex) const;

        [[nodiscard]] const std::vector<VkCommandBuffer>* getCommandBuffers() const;
    private:
        std::vector<VkCommandBuffer> commandBuffers;
    };
}