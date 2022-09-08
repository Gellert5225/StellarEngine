#include "stlrpch.h"
#include "CommandBuffer.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Log.h"

namespace Stellar {

    CommandBuffer::CommandBuffer(VkCommandPool commandPool, uint32_t size) {
        commandBuffers.resize(size);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        if (vkAllocateCommandBuffers(VulkanDevice::GetInstance()->logicalDevice(),
                                     &allocInfo, commandBuffers.data()) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate command buffers");
    }

    CommandBuffer::CommandBuffer(VkCommandPool commandPool, uint32_t size, VkCommandBufferLevel level) {
        commandBuffers.resize(size);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = level;
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

        if (vkAllocateCommandBuffers(VulkanDevice::GetInstance()->logicalDevice(),
                                     &allocInfo, commandBuffers.data()) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate command buffers");
    }

    void CommandBuffer::endRenderPass() const {
        for (auto& commandBuffer : commandBuffers)
            vkCmdEndRenderPass(commandBuffer);
    }

    void CommandBuffer::endCommandBuffer() const {
        for (auto &commandBuffer: commandBuffers) {
            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
                throw std::runtime_error("failed to record command buffer!");
        }
    }

    const std::vector<VkCommandBuffer> *CommandBuffer::getCommandBuffers() const {
        return &commandBuffers;
    }

    VkCommandBuffer CommandBuffer::getCurrentCommandBuffer(uint32_t currentFrameIndex) const {
        return commandBuffers[currentFrameIndex];
    }

    CommandBuffer::~CommandBuffer() {
        vkFreeCommandBuffers(VulkanDevice::GetInstance()->logicalDevice(),
                             VulkanDevice::GetInstance()->getCommandPool(),
                             commandBuffers.size(),
                             commandBuffers.data());
        commandBuffers.clear();
    }
}