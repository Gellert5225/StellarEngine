#pragma once

#include "Stellar/Core.h"
#include "Stellar/Platform/Vulkan/Instance/VulkanInstance.h"
#include "Stellar/Platform/Vulkan/SwapChain/SwapChain.h"
#include "Stellar/Platform/Vulkan/Queue/Queue.h"

#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API VulkanDevice {
    public:
        static VulkanDevice* GetInstance();
        ~VulkanDevice();

        void init(VkSurfaceKHR surfaceRef);

        [[nodiscard]] VkQueue& getGraphicsQueue() { return m_GraphicsQueue; }
        [[nodiscard]] VkQueue& getPresentQueue() { return m_PresentQueue; }
        [[nodiscard]] Queue::QueueFamilyIndices getIndices() const;
        [[nodiscard]] VkCommandPool getCommandPool() const;

        [[nodiscard]] VkPhysicalDevice physicalDevice() const;
        [[nodiscard]] VkDevice logicalDevice() const;

        [[nodiscard]] SwapChain::SwapChainSupportDetails getSwapChainSupport() const;

        [[nodiscard]] VkPhysicalDeviceProperties getDeviceProperties() const;

        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer);
    private:
        static VulkanDevice* s_Instance;

        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        VkDevice m_LogicalDevice = VK_NULL_HANDLE;

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_PresentQueue = VK_NULL_HANDLE;

        VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

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
