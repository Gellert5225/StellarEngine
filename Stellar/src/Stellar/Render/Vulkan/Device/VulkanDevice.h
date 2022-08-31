#pragma once

#include "Stellar/Core.h"
#include "Stellar/Render/Vulkan/Instance/VulkanInstance.h"
#include "Stellar/Render/Vulkan/SwapChain/SwapChain.h"
#include "Stellar/Render/Vulkan/Queue/Queue.h"

#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API VulkanDevice {
    public:
        static VulkanDevice* GetInstance();

        void init(VkSurfaceKHR);

        [[nodiscard]] VkQueue& getGraphicsQueue() { return graphicsQueue; }
        [[nodiscard]] VkQueue& getPresentQueue() { return presentQueue; }
        [[nodiscard]] Queue::QueueFamilyIndices getIndices() const;
        [[nodiscard]] VkCommandPool getCommandPool() const;

        [[nodiscard]] VkPhysicalDevice physicalDevice() const;
        [[nodiscard]] VkDevice logicalDevice() const;

        [[nodiscard]] SwapChain::SwapChainSupportDetails getSwapChainSupport() const;

        [[nodiscard]] VkPhysicalDeviceProperties getDeviceProperties() const;
        ~VulkanDevice();
    private:
        static VulkanDevice* s_Instance;

        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_LogicalDevice = VK_NULL_HANDLE;

        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;

        VkSurfaceKHR surface = VK_NULL_HANDLE;

        VkCommandPool m_CommandPool = VK_NULL_HANDLE;

        Queue::QueueFamilyIndices m_Indices;

        VulkanDevice() = default;
        bool isDeviceSuitable(VkPhysicalDevice device) const;
        bool checkDeviceExtensionSupport(VkPhysicalDevice device) const;

        void pickPhysicalDevice();
        void createLogicalDevice();
        void createCommandPool();

        Queue::QueueFamilyIndices findQueueFamilies(VkPhysicalDevice) const;
        SwapChain::SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice) const;
    };
}

