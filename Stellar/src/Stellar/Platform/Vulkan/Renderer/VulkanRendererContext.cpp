#include "stlrpch.h"

#include "VulkanRendererContext.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/SwapChain/VulkanSurface.h"

#include "Stellar/Core/Application.h"
#include "Stellar/Core/Log.h"

namespace Stellar {

    VulkanRendererContext::VulkanRendererContext() {
        VulkanInstance::GetInstance()->init("Stellar Engine Sandbox", 1, "Stellar", 1);
        VulkanSurface::GetInstance()->init(Application::Get().getWindow().getGLFWWindow());
        VulkanDevice::GetInstance()->init(VulkanSurface::GetInstance()->getSurface());
    }

    VulkanRendererContext::~VulkanRendererContext() {
        delete VulkanDevice::GetInstance();
        delete VulkanSurface::GetInstance();
        delete VulkanInstance::GetInstance();
    }

    std::string VulkanRendererContext::getGPUInfo() const {
        return VulkanDevice::GetInstance()->getDeviceProperties().deviceName;
    };

    std::string VulkanRendererContext::getGraphicsAPI() const {
        return VulkanInstance::GetInstance()->getInstanceVersion();
    };

//    VkCommandBuffer VulkanRendererContext::beginFrame() {
//        STLR_CORE_ASSERT(!m_IsFrameStarted,
//                         "VulkanRendererContext::beginFrame(): Frame already in progress")
//
//        VkResult result = m_SwapChain->acquireNextImage(&m_CurrentImageIndex);
//        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
//            recreateSwapChain();
//            return nullptr;
//        }
//
//        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
//            throw std::runtime_error("Failed to acquire swap chain image");
//
//        m_IsFrameStarted = true;
//
//        VkCommandBuffer currentCommandBuffer = getCurrentCommandBuffer();
//        VkCommandBufferBeginInfo beginInfo{};
//        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//
//        if (vkBeginCommandBuffer(currentCommandBuffer, &beginInfo) != VK_SUCCESS) {
//            throw std::runtime_error("failed to begin recording command buffer!");
//        }
//        return currentCommandBuffer;
//    }
//
//    void VulkanRendererContext::endFrame() {
//        STLR_CORE_ASSERT(m_IsFrameStarted,
//                         "VulkanRendererContext::endFrame(): Frame not in progress")
//
//        VkCommandBuffer currentCommandBuffer = getCurrentCommandBuffer();
//        if (vkEndCommandBuffer(currentCommandBuffer) != VK_SUCCESS)
//            throw std::runtime_error("failed to record command buffer!");
//
//        VkResult result = m_SwapChain->submitCommandBuffers(&currentCommandBuffer,
//                                                            &m_CurrentImageIndex);
//        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
//            recreateSwapChain();
//        } else if (result != VK_SUCCESS) {
//            throw std::runtime_error("failed to present swap chain image!");
//        }
//
//        m_IsFrameStarted = false;
//        m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % SwapChain::MAX_FRAMES_IN_FLIGHT;
//    }
//
////    VkCommandBuffer VulkanRendererContext::getCurrentCommandBuffer() const {
////        return m_CommandBuffer->getCurrentCommandBuffer(m_CurrentFrameIndex);
////    }
//
//    void VulkanRendererContext::beginRenderPass(VkCommandBuffer commandBuffer) {
//        STLR_CORE_ASSERT(m_IsFrameStarted,
//                         "VulkanRendererContext::beginRenderPass: Frame is not in progress");
//        STLR_CORE_ASSERT(commandBuffer == getCurrentCommandBuffer(),
//                         "VulkanRendererContext::beginRenderPass: Command buffer is from a different frame");
//
//        std::array<VkClearValue, 2> clearValues{};
//        clearValues[0].color = {{0.66f, 0.9f, 0.96f, 1.0f}};
//        clearValues[1].depthStencil = {1.0f, 0};
//
//        VkRenderPassBeginInfo renderPassInfo{};
//        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//        renderPassInfo.renderPass = m_SwapChain->getRenderPass();
//        renderPassInfo.framebuffer = getCurrentFrameBuffer();
//
//        renderPassInfo.renderArea.offset = {0, 0};
//        renderPassInfo.renderArea.extent = m_SwapChain->getSwapChainExtent();
//
//        renderPassInfo.clearValueCount = 1;
//        renderPassInfo.pClearValues = clearValues.data();
//
//        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
//
//        VkViewport viewport{};
//        viewport.x = 0.0f;
//        viewport.y = 0.0f;
//        viewport.width = static_cast<float>(m_SwapChain->getSwapChainExtent().width);
//        viewport.height = static_cast<float>(m_SwapChain->getSwapChainExtent().height);
//        viewport.minDepth = 0.0f;
//        viewport.maxDepth = 1.0f;
//        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
//
//        VkRect2D scissor{};
//        scissor.offset = {0, 0};
//        scissor.extent = m_SwapChain->getSwapChainExtent();
//        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
//
//        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
//                          *m_GraphicsPipeLine->getPipeline());
//    }
//
//    void VulkanRendererContext::endRenderPass(VkCommandBuffer commandBuffer) {
//        STLR_CORE_ASSERT(m_IsFrameStarted,
//                         "VulkanRendererContext::endRenderPass: Frame is not in progress")
//        STLR_CORE_ASSERT(commandBuffer == getCurrentCommandBuffer(),
//                         "VulkanRendererContext::endRenderPass: Command buffer is from a different frame")
//
//        vkCmdEndRenderPass(commandBuffer);
//    }

//    void VulkanRendererContext::beginImGUIFrame() {
//        VkCommandBufferBeginInfo cmdBufInfo = {};
//        cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//        cmdBufInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//        cmdBufInfo.pNext = nullptr;
//
//        VkCommandBuffer drawCommandBuffer = getCurrentCommandBuffer();
//        commandBuffer = drawCommandBuffer;
//        STLR_CORE_ASSERT(commandBuffer, "Command buffer is NULL!");
//        VK_CHECK_RESULT(vkBeginCommandBuffer(drawCommandBuffer, &cmdBufInfo));
//
//    }
}