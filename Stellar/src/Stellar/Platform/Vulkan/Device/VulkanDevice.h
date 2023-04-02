#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Platform/Vulkan/Instance/VulkanInstance.h"
#include "Stellar/Platform/Vulkan/SwapChain/VulkanSwapChain.h"
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

		[[nodiscard]] VulkanSwapChain::SwapChainSupportDetails getSwapChainSupport() const;

		[[nodiscard]] VkPhysicalDeviceProperties getDeviceProperties() const;
		[[nodiscard]] VkFormat getDepthFormat() const { return m_DepthFormat; }

		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer);
	private:
		static VulkanDevice* s_Instance;

		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_LogicalDevice = VK_NULL_HANDLE;
		VkFormat m_DepthFormat = VK_FORMAT_UNDEFINED;

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

		VkFormat findDepthFormat() const;

		Queue::QueueFamilyIndices findQueueFamilies(VkPhysicalDevice) const;
		VulkanSwapChain::SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice) const;
	};
}

