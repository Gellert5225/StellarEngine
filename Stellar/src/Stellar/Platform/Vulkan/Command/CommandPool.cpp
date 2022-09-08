#include "stlrpch.h"
#include "CommandPool.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

namespace Stellar {
    CommandPool* CommandPool::s_Instance = nullptr;

    CommandPool *CommandPool::GetInstance() {
        if (s_Instance == nullptr)
            s_Instance = new CommandPool();
        return s_Instance;
    }

    void CommandPool::init(Queue::QueueFamilyIndices indices) {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = indices.graphicsFamily.value();

        if (vkCreateCommandPool(VulkanDevice::GetInstance()->logicalDevice(), &poolInfo,
                                nullptr, &commandPool) != VK_SUCCESS)
            throw std::runtime_error("Failed to create command pool");
    }

    CommandPool::~CommandPool() {
        vkDestroyCommandPool(VulkanDevice::GetInstance()->logicalDevice(),
                             commandPool, nullptr);
    }

    VkCommandPool *CommandPool::getVkCommandPool() {
        return &commandPool;
    }

    void CommandPool::reset() {
        vkResetCommandPool(VulkanDevice::GetInstance()->logicalDevice(),
                           commandPool, VkCommandPoolResetFlags());
    }
}