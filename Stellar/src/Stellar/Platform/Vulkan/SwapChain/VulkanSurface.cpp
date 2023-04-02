#include "VulkanSurface.h"

#include "Stellar/Platform/Vulkan/Instance/VulkanInstance.h"

namespace Stellar {
	VulkanSurface* VulkanSurface::s_Instance = nullptr;
	VulkanSurface* VulkanSurface::GetInstance() {
		if (s_Instance == nullptr)
			s_Instance = new VulkanSurface();
		return s_Instance;
	}

	void VulkanSurface::init(GLFWwindow* window) {
		createSurface(window);
	}

	void VulkanSurface::createSurface(GLFWwindow* window) {
		if (glfwCreateWindowSurface(VulkanInstance::GetInstance()->getVkInstance(),
									window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	VulkanSurface::~VulkanSurface() {
		vkDestroySurfaceKHR(VulkanInstance::GetInstance()->getVkInstance(), surface, nullptr);
	}

	VkSurfaceKHR VulkanSurface::getSurface() const {
		return surface;
	}
}