#include "stlrpch.h"
#include "SwapChain.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "VulkanSurface.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"
#include "Stellar/Application.h"

#include "Stellar/Log.h"

namespace Stellar {
    SwapChain::SwapChain() {
        init();
    }

    void SwapChain::init() {
        createSwapChain();
        createImageViews();
        createRenderPass();
        createCommandBuffers();
        createFrameBuffers();
        createSemaphores();
    }

    VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
                availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
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
        for (auto& commandBuffer : m_CommandBuffers)
            vkDestroyCommandPool(VulkanDevice::GetInstance()->logicalDevice(), commandBuffer.CommandPool, nullptr);

        for (auto imageView : m_SwapChainImageViews) {
            vkDestroyImageView(VulkanDevice::GetInstance()->logicalDevice(),
                               imageView, nullptr);
        }
        m_SwapChainImageViews.clear();

        vkDestroySwapchainKHR(VulkanDevice::GetInstance()->logicalDevice(),
                              m_VulkanSwapChain, nullptr);

        for (auto& framebuffer : m_Framebuffers)
            vkDestroyFramebuffer(VulkanDevice::GetInstance()->logicalDevice(), framebuffer, nullptr);
        for (auto& framebuffer : m_ImGuiFramebuffers)
            vkDestroyFramebuffer(VulkanDevice::GetInstance()->logicalDevice(), framebuffer, nullptr);

        delete m_RenderPass;
        delete m_ImGuiRenderPass;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroyFence(VulkanDevice::GetInstance()->logicalDevice(),
                           m_InFlightFences[i], nullptr);
            vkDestroySemaphore(VulkanDevice::GetInstance()->logicalDevice(),
                               m_ImageAvailableSemaphores[i], nullptr);
            vkDestroySemaphore(VulkanDevice::GetInstance()->logicalDevice(),
                               m_RenderFinishedSemaphores[i], nullptr);
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

    VkCommandBuffer SwapChain::getCurrentCommandBuffer() const {
        return m_CommandBuffers[m_CurrentFrameIndex].CommandBuffer;
    }

    VkCommandBuffer SwapChain::getCommandBuffer(uint32_t index) const {
        return m_CommandBuffers[index].CommandBuffer;
    }

    VkFramebuffer SwapChain::getCurrentFrameBuffer() const {
        return m_Framebuffers[m_CurrentImageIndex];
    }

    uint32_t SwapChain::getCurrentFrameIndex() const {
        return m_CurrentFrameIndex;
    }

    VkRenderPass SwapChain::getRenderPass() const {
        return m_RenderPass->getVkRenderPass();
    }

    uint32_t SwapChain::getImageCount() const {
        return m_SwapChainImages.size();
    }

    VkRenderPass SwapChain::getImGuiRenderPass() const {
        return m_ImGuiRenderPass->getVkRenderPass();
    }

    VkFramebuffer SwapChain::getCurrentImGuiFrameBuffer() const {
        return m_ImGuiFramebuffers[m_CurrentImageIndex];
    }

    void SwapChain::createCommandBuffers() {
        for (auto& commandBuffer : m_CommandBuffers)
            vkDestroyCommandPool(VulkanDevice::GetInstance()->logicalDevice(),
                                 commandBuffer.CommandPool, nullptr);

        VkCommandPoolCreateInfo cmdPoolInfo = {};
        cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolInfo.queueFamilyIndex = VulkanDevice::GetInstance()->getIndices().graphicsFamily.value();
        cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = 1;

        m_CommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);
        for (auto& commandBuffer : m_CommandBuffers) {
            VK_CHECK_RESULT(vkCreateCommandPool(VulkanDevice::GetInstance()->logicalDevice(), &cmdPoolInfo, nullptr, &commandBuffer.CommandPool));

            commandBufferAllocateInfo.commandPool = commandBuffer.CommandPool;
            VK_CHECK_RESULT(vkAllocateCommandBuffers(VulkanDevice::GetInstance()->logicalDevice(), &commandBufferAllocateInfo, &commandBuffer.CommandBuffer));
        }
    }

    void SwapChain::createImageViews() {
        for (auto imageView : m_SwapChainImageViews) {
            vkDestroyImageView(VulkanDevice::GetInstance()->logicalDevice(),
                               imageView, nullptr);
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

        VkSwapchainKHR oldSwapchain = m_VulkanSwapChain;

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
        createInfo.oldSwapchain = oldSwapchain;

        if (vkCreateSwapchainKHR(VulkanDevice::GetInstance()->logicalDevice(),
                                 &createInfo, nullptr, &m_VulkanSwapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        if (oldSwapchain)
            vkDestroySwapchainKHR(VulkanDevice::GetInstance()->logicalDevice(),
                                  oldSwapchain, nullptr);

        vkGetSwapchainImagesKHR(VulkanDevice::GetInstance()->logicalDevice(),
                                m_VulkanSwapChain, &imageCount, nullptr);
        m_SwapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(VulkanDevice::GetInstance()->logicalDevice(),
                                m_VulkanSwapChain, &imageCount, m_SwapChainImages.data());

        m_SwapChainImageFormat = surfaceFormat.format;
        m_SwapChainExtent = extent;
    }

    void SwapChain::createSemaphores() {
        m_ImagesInFlight.resize(getImageCount(), VK_NULL_HANDLE);
        if (m_ImageAvailableSemaphores.size() != MAX_FRAMES_IN_FLIGHT ||
            m_RenderFinishedSemaphores.size() != MAX_FRAMES_IN_FLIGHT) {
            m_ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
            m_RenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                vkCreateSemaphore(VulkanDevice::GetInstance()->logicalDevice(),
                                  &semaphoreInfo,
                                  nullptr,
                                  &m_ImageAvailableSemaphores[i]);
                vkCreateSemaphore(VulkanDevice::GetInstance()->logicalDevice(),
                                  &semaphoreInfo,
                                  nullptr,
                                  &m_RenderFinishedSemaphores[i]);
            }
        }

        if (m_InFlightFences.size() != MAX_FRAMES_IN_FLIGHT) {
            m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                if (vkCreateFence(VulkanDevice::GetInstance()->logicalDevice(),
                                  &fenceInfo,
                                  nullptr,
                                  &m_InFlightFences[i]) != VK_SUCCESS) {
                    throw std::runtime_error("failed to create semaphores!");
                }
            }
        }
    }

    void SwapChain::createFrameBuffers() {
        for (auto& framebuffer : m_Framebuffers)
            vkDestroyFramebuffer(VulkanDevice::GetInstance()->logicalDevice(), framebuffer, nullptr);
        for (auto& framebuffer : m_ImGuiFramebuffers)
            vkDestroyFramebuffer(VulkanDevice::GetInstance()->logicalDevice(), framebuffer, nullptr);

        VkFramebufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo.renderPass = m_RenderPass->getVkRenderPass();
        frameBufferCreateInfo.attachmentCount = 1;
        frameBufferCreateInfo.width = m_SwapChainExtent.width;
        frameBufferCreateInfo.height = m_SwapChainExtent.height;
        frameBufferCreateInfo.layers = 1;

        VkFramebufferCreateInfo frameBufferCreateInfo2 = {};
        frameBufferCreateInfo2.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo2.renderPass = m_ImGuiRenderPass->getVkRenderPass();
        frameBufferCreateInfo2.attachmentCount = 1;
        frameBufferCreateInfo2.width = m_SwapChainExtent.width;
        frameBufferCreateInfo2.height = m_SwapChainExtent.height;
        frameBufferCreateInfo2.layers = 1;

        m_Framebuffers.resize(getImageCount());
        m_ImGuiFramebuffers.resize(getImageCount());
        for (uint32_t i = 0; i < m_Framebuffers.size(); i++) {
            frameBufferCreateInfo.pAttachments = &m_SwapChainImageViews[i];
            frameBufferCreateInfo2.pAttachments = &m_SwapChainImageViews[i];
            VK_CHECK_RESULT(vkCreateFramebuffer(VulkanDevice::GetInstance()->logicalDevice(), &frameBufferCreateInfo, nullptr, &m_Framebuffers[i]));
            VK_CHECK_RESULT(vkCreateFramebuffer(VulkanDevice::GetInstance()->logicalDevice(), &frameBufferCreateInfo2, nullptr, &m_ImGuiFramebuffers[i]));
        }

    }

    void SwapChain::createRenderPass() {
        delete m_RenderPass;
        delete m_ImGuiRenderPass;
        m_RenderPass = new StandardRenderPass(m_SwapChainImageFormat);
        m_ImGuiRenderPass = new ImGuiRenderPass(m_SwapChainImageFormat);
    }

    void SwapChain::beginFrame() {
        STLR_CORE_ASSERT(!m_IsFrameStarted,
                         "VulkanRendererContext::beginFrame(): Frame already in progress")

        VkResult result = vkAcquireNextImageKHR(VulkanDevice::GetInstance()->logicalDevice(),
                                                m_VulkanSwapChain,
                                                UINT64_MAX,
                                                m_ImageAvailableSemaphores[m_CurrentFrameIndex],
                                                VK_NULL_HANDLE,
                                                &m_CurrentImageIndex);

        vkResetCommandPool(VulkanDevice::GetInstance()->logicalDevice(),
                           m_CommandBuffers[m_CurrentFrameIndex].CommandPool,
                           0);

        if (result == VK_ERROR_OUT_OF_DATE_KHR)
            onResize();

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
            throw std::runtime_error("Failed to acquire swap chain image");

        m_IsFrameStarted = true;
    }

    void SwapChain::present() {
        if (m_ImagesInFlight[m_CurrentImageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(VulkanDevice::GetInstance()->logicalDevice(), 1, &m_ImagesInFlight[m_CurrentImageIndex], VK_TRUE, UINT64_MAX);
        }
        m_ImagesInFlight[m_CurrentImageIndex] = m_InFlightFences[m_CurrentFrameIndex];

        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &m_ImageAvailableSemaphores[m_CurrentFrameIndex];
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrameIndex].CommandBuffer;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrameIndex];

        vkResetFences(VulkanDevice::GetInstance()->logicalDevice(),
                      1,  &m_InFlightFences[m_CurrentFrameIndex]);
        if (vkQueueSubmit(VulkanDevice::GetInstance()->getGraphicsQueue(),
                          1,
                          &submitInfo,
                          m_InFlightFences[m_CurrentFrameIndex]) != VK_SUCCESS) {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphores[m_CurrentFrameIndex];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_VulkanSwapChain;
        presentInfo.pImageIndices = &m_CurrentImageIndex;
        presentInfo.pResults = nullptr;

        VkResult result = vkQueuePresentKHR(VulkanDevice::GetInstance()->getPresentQueue(),
                                            &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            onResize();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        m_IsFrameStarted = false;
        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;

        vkWaitForFences(VulkanDevice::GetInstance()->logicalDevice(),
                        1,
                        &m_InFlightFences[m_CurrentFrameIndex],
                        VK_TRUE,
                        UINT64_MAX);
    }

    void SwapChain::onResize() {
        vkDeviceWaitIdle(VulkanDevice::GetInstance()->logicalDevice());
        init();
        vkDeviceWaitIdle(VulkanDevice::GetInstance()->logicalDevice());
    }

    //    VkResult SwapChain::acquireNextImage(uint32_t* imageIndex) {
//        // wait for previous frame
////        vkWaitForFences(VulkanDevice::GetInstance()->logicalDevice(),
////                        1,
////                        &m_InFlightFences[m_CurrentFrameIndex],
////                        VK_TRUE,
////                        UINT64_MAX);
//
//        VkResult result = vkAcquireNextImageKHR(VulkanDevice::GetInstance()->logicalDevice(),
//                              m_VulkanSwapChain,
//                              UINT64_MAX,
//                              m_ImageAvailableSemaphores[m_CurrentFrameIndex],
//                              VK_NULL_HANDLE, imageIndex);
//
//        return result;
//    }

//    VkResult SwapChain::submitCommandBuffers(const VkCommandBuffer* buffer, const uint32_t* imageIndex) {
////        if (m_ImagesInFlight[*imageIndex] != VK_NULL_HANDLE) {
////            vkWaitForFences(VulkanDevice::GetInstance()->logicalDevice(),
////                            1, &m_ImagesInFlight[*imageIndex], VK_TRUE, UINT64_MAX);
////        }
////        m_ImagesInFlight[*imageIndex] = m_ImagesInFlight[m_CurrentFrameIndex];
//
//        //submit command buffer
//        VkSubmitInfo submitInfo{};
//        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//
//        VkSemaphore waitSemaphores[] = {m_ImageAvailableSemaphores[m_CurrentFrameIndex]};
//        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
//        submitInfo.waitSemaphoreCount = 1;
//        submitInfo.pWaitSemaphores = waitSemaphores;
//        submitInfo.pWaitDstStageMask = waitStages;
//        submitInfo.commandBufferCount = 1;
//        submitInfo.pCommandBuffers = buffer;
//
//        VkSemaphore signalSemaphores[] = {m_RenderFinishedSemaphores[m_CurrentFrameIndex]};
//        submitInfo.signalSemaphoreCount = 1;
//        submitInfo.pSignalSemaphores = signalSemaphores;
//
//        vkResetFences(VulkanDevice::GetInstance()->logicalDevice(),
//                      1,  &m_InFlightFences[m_CurrentFrameIndex]);
//        if (vkQueueSubmit(VulkanDevice::GetInstance()->getGraphicsQueue(),
//                          1,
//                          &submitInfo,
//                          m_InFlightFences[m_CurrentFrameIndex]) != VK_SUCCESS) {
//            throw std::runtime_error("failed to submit draw command buffer!");
//        }
//
//        VkPresentInfoKHR presentInfo{};
//        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//
//        presentInfo.waitSemaphoreCount = 1;
//        presentInfo.pWaitSemaphores = signalSemaphores;
//
//        VkSwapchainKHR swapChains[] = {m_VulkanSwapChain};
//        presentInfo.swapchainCount = 1;
//        presentInfo.pSwapchains = swapChains;
//        presentInfo.pImageIndices = imageIndex;
//        presentInfo.pResults = nullptr;
//
//        VkResult result = vkQueuePresentKHR(VulkanDevice::GetInstance()->getPresentQueue(),
//                                            &presentInfo);
//
//       // m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
//
//        return result;
//    }

}