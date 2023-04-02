#pragma once

#include "Stellar/Core/Core.h"

#include <vulkan/vulkan.h>

namespace Stellar {
	class STLR_API VulkanValidationLayer {
	public:
		VulkanValidationLayer();
		~VulkanValidationLayer();

		void setupDebugMessenger();
		[[nodiscard]] static bool ValidationLayerEnabled();
		[[nodiscard]] static bool CheckValidationLayerSupport();
		static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT&);
	private:
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
				VkDebugUtilsMessageSeverityFlagBitsEXT,
				VkDebugUtilsMessageTypeFlagsEXT,
				const VkDebugUtilsMessengerCallbackDataEXT*,
				void*);
		static VkResult CreateDebugUtilsMessengerEXT(
				VkInstance,
				const VkDebugUtilsMessengerCreateInfoEXT*,
				const VkAllocationCallbacks*,
				VkDebugUtilsMessengerEXT*);

		static void DestroyDebugUtilsMessengerEXT(VkInstance,
												VkDebugUtilsMessengerEXT,
												const VkAllocationCallbacks*);
	};
}
