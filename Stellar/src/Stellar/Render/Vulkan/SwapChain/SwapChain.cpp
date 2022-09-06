#include "stlrpch.h"
#include "SwapChain.h"
#include "Stellar/Render/Vulkan/Device/VulkanDevice.h"
#include "VulkanSurface.h"
#include "Stellar/Application.h"

#include "Stellar/Log.h"

namespace Stellar {
    SwapChain::SwapChain() {
        createSwapChain();
        createImageViews();
        createRenderPass();
        createFrameBuffers();
        createSemaphores();
    }

    SwapChain::SwapChain(std::shared_ptr<SwapChain> oldSwapChain) : m_OldSwapChain(oldSwapChain) {
        createSwapChain();
        createImageViews();
        createRenderPass();
        createFrameBuffers();
        createSemaphores();
        oldSwapChain = nullptr;
    }

    VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(
            const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;
        }
        return availableFormats[0];
    }

    VkPresentModeKHR SwapChain::chooseSwapPresentMode(
            const std::vector<VkPresentModeKHR>& availblePresentModes) {
        for (const auto& availablePresentMode : availblePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                return availablePresentMode;
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
            return capabilities.currentExtent;
        } else {
            int width, height;
            glfwGetFramebufferSize(Application::Get().getWindow().getGLFWWindow(),
                                   &width, &height);

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

    SwapChain::~SwapChain() {
        for (auto imageView : m_SwapChainImageViews) {
            vkDestroyImageView(VulkanDevice::GetInstance()->logicalDevice(),
                               imageView, nullptr);
        }
        m_SwapChainImageViews.clear();

        vkDestroySwapchainKHR(VulkanDevice::GetInstance()->logicalDevice(),
                              m_VulkanSwapChain, nullptr);

        delete m_FrameBuffer;
        delete m_RenderPass;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(VulkanDevice::GetInstance()->logicalDevice(),
                               m_ImageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(VulkanDevice::GetInstance()->logicalDevice(),
                               m_RenderFinishedSemaphores[i], nullptr);
            vkDestroyFence(VulkanDevice::GetInstance()->logicalDevice(),
                           m_InFlightFences[i], nullptr);
        }
    }

    const std::vector<VkImage>* SwapChain::getSwapChainImages() const {
        return &m_SwapChainImages;
    }

    VkFormat SwapChain::getSwapChainImageFormat() const {
        return m_SwapChainImageFormat;
    }

    VkExtent2D SwapChain::getSwapChainExtent() const {
        return m_SwapChainExtent;
    }

    VkSwapchainKHR& SwapChain::getSwapChain() {
        return m_VulkanSwapChain;
    }

    void SwapChain::createImageViews() {
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

            if (vkCreateImageView(VulkanDevice::GetInstance()->logicalDevice(), &createInfo,
                                  nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    void SwapChain::createSwapChain() {
        SwapChainSupportDetails support = VulkanDevice::GetInstance()->getSwapChainSupport();
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(support.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(support.presentModes);
        VkExtent2D extent = chooseSwapExtent(support.capabilities);

        uint32_t imageCount = support.capabilities.minImageCount + 1;

        if (support.capabilities.maxImageCount > 0
            && imageCount > support.capabilities.maxImageCount)
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
        createInfo.oldSwapchain = m_OldSwapChain == nullptr ? VK_NULL_HANDLE : m_OldSwapChain->getSwapChain();

        if (vkCreateSwapchainKHR(VulkanDevice::GetInstance()->logicalDevice(),
                                 &createInfo, nullptr, &m_VulkanSwapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(VulkanDevice::GetInstance()->logicalDevice(),
                                m_VulkanSwapChain, &imageCount, nullptr);
        m_SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(VulkanDevice::GetInstance()->logicalDevice(),
                                m_VulkanSwapChain, &imageCount, m_SwapChainImages.data());

        m_SwapChainImageFormat = surfaceFormat.format;
        m_SwapChainExtent = extent;
    }

    void SwapChain::createSemaphores() {
        m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
        m_ImagesInFlight.resize(m_SwapChainImages.size(), VK_NULL_HANDLE);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(VulkanDevice::GetInstance()->logicalDevice(),
                                  &semaphoreInfo,
                                  nullptr,
                                  &m_ImageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(VulkanDevice::GetInstance()->logicalDevice(),
                                  &semaphoreInfo,
                                  nullptr,
                                  &m_RenderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(VulkanDevice::GetInstance()->logicalDevice(),
                              &fenceInfo,
                              nullptr,
                              &m_InFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create semaphores!");
            }
        }
    }

    void SwapChain::createFrameBuffers() {
        m_FrameBuffer = new FrameBuffer(m_SwapChainImageViews,
                                        m_SwapChainExtent, m_RenderPass->getVkRenderPass());
    }

    void SwapChain::createRenderPass() {
        m_RenderPass = new StandardRenderPass(m_SwapChainImageFormat);
    }

    VkRenderPass SwapChain::getRenderPass() const {
        return m_RenderPass->getVkRenderPass();
    }

    VkFramebuffer SwapChain::getFrameBuffer(uint32_t index) const {
        return (*m_FrameBuffer->getFramebuffers())[index];
    }

    uint32_t SwapChain::getImageCount() const {
        return m_SwapChainImages.size();
    }

    VkResult SwapChain::acquireNextImage(uint32_t* imageIndex) {
        // wait for previous frame
        vkWaitForFences(VulkanDevice::GetInstance()->logicalDevice(),
                        1,
                        &m_InFlightFences[m_CurrentFrame],
                        VK_TRUE,
                        UINT64_MAX);

        VkResult result = vkAcquireNextImageKHR(VulkanDevice::GetInstance()->logicalDevice(),
                              m_VulkanSwapChain,
                              UINT64_MAX,
                              m_ImageAvailableSemaphores[m_CurrentFrame],
                              VK_NULL_HANDLE, imageIndex);

        return result;
    }

    VkResult SwapChain::submitCommandBuffers(const VkCommandBuffer* buffer, const uint32_t* imageIndex) {
        if (m_ImagesInFlight[*imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(VulkanDevice::GetInstance()->logicalDevice(),
                            1, &m_ImagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
        }
        m_ImagesInFlight[*imageIndex] = m_ImagesInFlight[m_CurrentFrame];

        //submit command buffer
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = buffer;

        VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(VulkanDevice::GetInstance()->logicalDevice(),
                      1,  &m_InFlightFences[m_CurrentFrame]);
        if (vkQueueSubmit(VulkanDevice::GetInstance()->getGraphicsQueue(),
                          1,
                          &submitInfo,
                          m_InFlightFences[m_CurrentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {m_VulkanSwapChain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = imageIndex;
        presentInfo.pResults = nullptr;

        VkResult result = vkQueuePresentKHR(VulkanDevice::GetInstance()->getPresentQueue(),
                                            &presentInfo);

        m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

        return result;
    }

    bool SwapChain::compareSwapFormats(const SwapChain &swapChain) const {
        return swapChain.m_SwapChainImageFormat == m_SwapChainImageFormat;
    }
}