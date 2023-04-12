#include "stlrpch.h"

#include "VulkanSwapChain.h"
#include "VulkanSurface.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"
#include "Stellar/Core/Application.h"
#include "Stellar/Core/Log.h"

namespace Stellar {
	VulkanSwapChain::VulkanSwapChain() {
		init();
	}

	void VulkanSwapChain::init() {
		createSwapChain();
		createImageViews();
		createRenderPass();
		createCommandBuffers();
		createFrameBuffers();
		createSemaphores();
	}

	VkSurfaceFormatKHR VulkanSwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
				availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
				return availableFormat;
		}
		return availableFormats[0];
	}

	VkPresentModeKHR VulkanSwapChain::chooseSwapPresentMode(
			const std::vector<VkPresentModeKHR>& availblePresentModes) {
		for (const auto& availablePresentMode : availblePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return availablePresentMode;
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		} else {
			int width, height;
			glfwGetFramebufferSize(Application::Get().getWindow().getGLFWWindow(), &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width,
											capabilities.minImageExtent.width,
											capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height,
											capabilities.minImageExtent.height,
											capabilities.maxImageExtent.height);
			return actualExtent;
		}
	}

	VulkanSwapChain::~VulkanSwapChain() {
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		// for (auto& commandBuffer : m_CommandBuffers)
		//     vkDestroyCommandPool(VulkanDevice::GetInstance()->logicalDevice(), commandBuffer.CommandPool, nullptr);

		for (auto imageView : m_SwapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}
		m_SwapChainImageViews.clear();

		vkDestroySwapchainKHR(device, m_VulkanSwapChain, nullptr);

		for (auto& framebuffer : m_Framebuffers)
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		delete m_ImGuiRenderPass;

		for (size_t i = 0; i < Renderer::MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroyFence(device, m_InFlightFences[i], nullptr);
			vkDestroySemaphore(device, m_ImageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(device, m_RenderFinishedSemaphores[i], nullptr);
		}
	}

	const std::vector<VkImage>* VulkanSwapChain::getSwapChainImages() const {
		return &m_SwapChainImages;
	}

	VkFormat VulkanSwapChain::getSwapChainImageFormat() const {
		return m_SwapChainImageFormat;
	}

	SwapChain::SwapChainExtent2D VulkanSwapChain::getSwapChainExtent() const {
		return m_SwapChainExtent;
	}

	VkSwapchainKHR& VulkanSwapChain::getSwapChain() {
		return m_VulkanSwapChain;
	}

	VkCommandBuffer VulkanSwapChain::getCurrentCommandBuffer() const {
		return m_CommandBuffers[m_CurrentFrameIndex];
	}

	VkCommandBuffer VulkanSwapChain::getCommandBuffer(uint32_t index) const {
		return m_CommandBuffers[index];
	}

	VkFramebuffer VulkanSwapChain::getCurrentFrameBuffer() const {
		return m_Framebuffers[m_CurrentImageIndex];
	}

	uint32_t VulkanSwapChain::getCurrentFrameIndex() const {
		return m_CurrentFrameIndex;
	}

	uint32_t VulkanSwapChain::getImageCount() const {
		return m_SwapChainImages.size();
	}

	VkRenderPass VulkanSwapChain::getImGuiRenderPass() const {
		return m_ImGuiRenderPass->getVkRenderPass();
	}

	void VulkanSwapChain::createCommandBuffers() {
		// for (auto& commandBuffer : m_CommandBuffers)
		//     vkDestroyCommandPool(VulkanDevice::GetInstance()->logicalDevice(),
		//                          commandBuffer.CommandPool, nullptr);

		auto device = VulkanDevice::GetInstance()->logicalDevice();
		// VkCommandPoolCreateInfo cmdPoolInfo = {};
		// cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		// cmdPoolInfo.queueFamilyIndex = VulkanDevice::GetInstance()->getIndices().graphicsFamily.value();
		// cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;
		commandBufferAllocateInfo.commandPool = VulkanDevice::GetInstance()->getCommandPool();

		m_CommandBuffers.resize(Renderer::MAX_FRAMES_IN_FLIGHT);
		for (auto& commandBuffer : m_CommandBuffers) {
			//VK_CHECK_RESULT(vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &commandBuffer.CommandPool))

			VK_CHECK_RESULT(vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, &commandBuffer))
		}
	}

	void VulkanSwapChain::createImageViews() {
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		for (auto imageView : m_SwapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}
		m_SwapChainImageViews.clear();

		m_SwapChainImageViews.resize(m_SwapChainImages.size());

		for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_SwapChainImages[i];
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_SwapChainImageFormat;

			VK_CHECK_RESULT(vkCreateImageView(device, &createInfo, nullptr, &m_SwapChainImageViews[i]));
		}
	}

	void VulkanSwapChain::createSwapChain() {
		auto device = VulkanDevice::GetInstance()->logicalDevice();

		SwapChainSupportDetails support = VulkanDevice::GetInstance()->getSwapChainSupport();
		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(support.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(support.presentModes);
		VkExtent2D extent = chooseSwapExtent(support.capabilities);

		VkSwapchainKHR oldSwapchain = m_VulkanSwapChain;

		uint32_t imageCount = support.capabilities.minImageCount + 1;

		if (support.capabilities.maxImageCount > 0 && imageCount > support.capabilities.maxImageCount)
			imageCount = support.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = VulkanSurface::GetInstance()->getSurface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		Queue::QueueFamilyIndices indices = VulkanDevice::GetInstance()->getIndices();

		uint32_t queueFamilyIndices[] = {
				indices.graphicsFamily.value(),
				indices.presentFamily.value()
		};

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = support.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = oldSwapchain;

		VK_CHECK_RESULT(vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_VulkanSwapChain));

		if (oldSwapchain)
			vkDestroySwapchainKHR(device, oldSwapchain, nullptr);

		vkGetSwapchainImagesKHR(device, m_VulkanSwapChain, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device,  m_VulkanSwapChain, &imageCount, m_SwapChainImages.data());

		m_SwapChainImageFormat = surfaceFormat.format;
		m_SwapChainExtent = { extent.width, extent.height };
	}

	void VulkanSwapChain::createSemaphores() {
		auto device = VulkanDevice::GetInstance()->logicalDevice();

		m_ImagesInFlight.resize(getImageCount(), VK_NULL_HANDLE);
		if (m_ImageAvailableSemaphores.size() != Renderer::MAX_FRAMES_IN_FLIGHT ||
			m_RenderFinishedSemaphores.size() != Renderer::MAX_FRAMES_IN_FLIGHT) {
			m_ImageAvailableSemaphores.resize(Renderer::MAX_FRAMES_IN_FLIGHT);
			m_RenderFinishedSemaphores.resize(Renderer::MAX_FRAMES_IN_FLIGHT);

			VkSemaphoreCreateInfo semaphoreInfo{};
			semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			for (size_t i = 0; i < Renderer::MAX_FRAMES_IN_FLIGHT; i++) {
				vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphores[i]);
				vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphores[i]);
			}
		}

		if (m_InFlightFences.size() != Renderer::MAX_FRAMES_IN_FLIGHT) {
			m_InFlightFences.resize(Renderer::MAX_FRAMES_IN_FLIGHT);

			VkFenceCreateInfo fenceInfo{};
			fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			for (size_t i = 0; i < Renderer::MAX_FRAMES_IN_FLIGHT; i++) {
				VK_CHECK_RESULT(vkCreateFence(device, &fenceInfo,nullptr, &m_InFlightFences[i]));
			}
		}
	}

	void VulkanSwapChain::createFrameBuffers() {
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		for (auto& framebuffer : m_Framebuffers)
			vkDestroyFramebuffer(device, framebuffer, nullptr);

		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = m_ImGuiRenderPass->getVkRenderPass();
		info.attachmentCount = 1;
		info.width = m_SwapChainExtent.width;
		info.height = m_SwapChainExtent.height;
		info.layers = 1;

		m_Framebuffers.resize(getImageCount());
		for (uint32_t i = 0; i < m_Framebuffers.size(); i++) {
			info.pAttachments = &m_SwapChainImageViews[i];
			VK_CHECK_RESULT(vkCreateFramebuffer(device, &info, nullptr,  &m_Framebuffers[i]))
		}

	}

	void VulkanSwapChain::createRenderPass() {
		delete m_ImGuiRenderPass;
		m_ImGuiRenderPass = new ImGuiRenderPass(m_SwapChainImageFormat);
	}

	void VulkanSwapChain::beginFrame() {
		STLR_CORE_ASSERT(!m_IsFrameStarted, "Frame already in progress")

		VkResult result = vkAcquireNextImageKHR(VulkanDevice::GetInstance()->logicalDevice(),
												m_VulkanSwapChain,
												UINT64_MAX,
												m_ImageAvailableSemaphores[m_CurrentFrameIndex],
												VK_NULL_HANDLE,
												&m_CurrentImageIndex);

		// vkResetCommandPool(VulkanDevice::GetInstance()->logicalDevice(),
		//                    m_CommandBuffers[m_CurrentFrameIndex],
		//                    0);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
			onResize();

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("Failed to acquire swap chain image");

		m_IsFrameStarted = true;
	}

	void VulkanSwapChain::present() {
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		if (m_ImagesInFlight[m_CurrentImageIndex] != VK_NULL_HANDLE) {
			vkWaitForFences(device, 1, &m_ImagesInFlight[m_CurrentImageIndex], VK_TRUE, UINT64_MAX);
		}
		m_ImagesInFlight[m_CurrentImageIndex] = m_InFlightFences[m_CurrentFrameIndex];

		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &m_ImageAvailableSemaphores[m_CurrentFrameIndex];
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrameIndex];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrameIndex];

		vkResetFences(device, 1,  &m_InFlightFences[m_CurrentFrameIndex]);
		VK_CHECK_RESULT(
			vkQueueSubmit(VulkanDevice::GetInstance()->getGraphicsQueue(),
						1, &submitInfo, m_InFlightFences[m_CurrentFrameIndex])
		);

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrameIndex];
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_VulkanSwapChain;
		presentInfo.pImageIndices = &m_CurrentImageIndex;
		presentInfo.pResults = nullptr;

		VkResult result = vkQueuePresentKHR(VulkanDevice::GetInstance()->getPresentQueue(), &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			onResize();
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		m_IsFrameStarted = false;
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % Renderer::MAX_FRAMES_IN_FLIGHT;

		vkWaitForFences(device, 1, &m_InFlightFences[m_CurrentFrameIndex], VK_TRUE, UINT64_MAX);
		vkDeviceWaitIdle(device);
	}

	void VulkanSwapChain::onResize() {
		vkDeviceWaitIdle(VulkanDevice::GetInstance()->logicalDevice());
		init();
		vkDeviceWaitIdle(VulkanDevice::GetInstance()->logicalDevice());
	}
}