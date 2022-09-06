#pragma once

#include "Stellar/Core.h"
#include "CommandPool.h"
#include "Stellar/Render/Vulkan/RenderPass/StandardRenderPass.h"
#include "Stellar/Render/Vulkan/Pipeline/GraphicsPipeline.h"
#include "Stellar/Render/Vulkan/Buffer/Buffer.h"
#include "Stellar/Render/Vulkan/Buffer/FrameBuffer.h"
#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API CommandBuffer {
    public:
        CommandBuffer(VkCommandPool commandPool, uint32_t size);
        CommandBuffer(VkCommandPool commandPool, uint32_t size, VkCommandBufferLevel level);
        ~CommandBuffer() = default;

        void endRenderPass() const;
        void endCommandBuffer() const;

        [[nodiscard]] VkCommandBuffer getCurrentCommandBuffer(int currentFrameIndex) const;

        [[nodiscard]] const std::vector<VkCommandBuffer>* getCommandBuffers() const;
    private:
        std::vector<VkCommandBuffer> commandBuffers;
    };
}