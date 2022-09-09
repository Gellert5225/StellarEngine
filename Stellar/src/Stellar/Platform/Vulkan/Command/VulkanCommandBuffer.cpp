#include "stlrpch.h"
#include "VulkanCommandBuffer.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Log.h"
#include "Stellar/Application.h"
#include "Stellar/Renderer/Renderer.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"

namespace Stellar {

    VulkanCommandBuffer::VulkanCommandBuffer(uint32_t size) {
        m_CommandBuffers.resize(size);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = VulkanDevice::GetInstance()->getCommandPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t) m_CommandBuffers.size();

        if (vkAllocateCommandBuffers(VulkanDevice::GetInstance()->logicalDevice(),
                                     &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate command buffers");

        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        m_WaitFences.resize(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (auto & m_WaitFence : m_WaitFences) {
            VK_CHECK_RESULT(vkCreateFence(VulkanDevice::GetInstance()->logicalDevice(),
                                          &fenceCreateInfo, nullptr, &m_WaitFence));
        }

    }

    VulkanCommandBuffer::VulkanCommandBuffer(uint32_t size, VkCommandBufferLevel level) {
        m_CommandBuffers.resize(size);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = VulkanDevice::GetInstance()->getCommandPool();
        allocInfo.level = level;
        allocInfo.commandBufferCount = (uint32_t) m_CommandBuffers.size();

        if (vkAllocateCommandBuffers(VulkanDevice::GetInstance()->logicalDevice(),
                                     &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
            throw std::runtime_error("Failed to allocate command buffers");
    }

    const std::vector<VkCommandBuffer> *VulkanCommandBuffer::getCommandBuffers() const {
        return &m_CommandBuffers;
    }

    VkCommandBuffer VulkanCommandBuffer::getCurrentCommandBuffer(uint32_t currentFrameIndex) const {
        return m_CommandBuffers[currentFrameIndex];
    }

    VulkanCommandBuffer::~VulkanCommandBuffer() {
        vkFreeCommandBuffers(VulkanDevice::GetInstance()->logicalDevice(),
                             VulkanDevice::GetInstance()->getCommandPool(),
                             m_CommandBuffers.size(),
                             m_CommandBuffers.data());
        m_CommandBuffers.clear();

        for (auto & m_WaitFence : m_WaitFences) {
            vkDestroyFence(VulkanDevice::GetInstance()->logicalDevice(),
                           m_WaitFence, nullptr);
        }
    }

    void VulkanCommandBuffer::begin() {
        uint32_t frameIndex = Renderer::GetCurrentFrameIndex();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        beginInfo.pNext = nullptr;

        VkCommandBuffer commandBuffer = nullptr;
        commandBuffer = m_CommandBuffers[frameIndex];//swapChain->getCommandBuffer(frameIndex);
        m_ActiveCommandBuffer = commandBuffer;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
    }

    void VulkanCommandBuffer::end() {
        auto commandBuffer = static_cast<VkCommandBuffer>(m_ActiveCommandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
            throw std::runtime_error("failed to record command buffer!");
        m_ActiveCommandBuffer = nullptr;
    }

    void VulkanCommandBuffer::submit() {
        uint32_t frameIndex = Renderer::GetCurrentFrameIndex();

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        VkCommandBuffer commandBuffer = m_CommandBuffers[frameIndex];
        submitInfo.pCommandBuffers = &commandBuffer;

        VK_CHECK_RESULT(vkWaitForFences(VulkanDevice::GetInstance()->logicalDevice(),
                                        1,
                                        &m_WaitFences[frameIndex], VK_TRUE, UINT64_MAX));
        VK_CHECK_RESULT(vkResetFences(VulkanDevice::GetInstance()->logicalDevice(),
                                      1, &m_WaitFences[frameIndex]));
        VK_CHECK_RESULT(vkQueueSubmit(VulkanDevice::GetInstance()->getGraphicsQueue(),
                                      1, &submitInfo, m_WaitFences[frameIndex]));
    }

    void *VulkanCommandBuffer::getActiveCommandBuffer() {
        return m_ActiveCommandBuffer;
    }
}