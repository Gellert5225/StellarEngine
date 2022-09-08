#include "VulkanRenderer.h"

namespace Stellar {

    void VulkanRenderer::init() {
        SwapChain* swapChain = Application::Get().getWindow().getSwapChain();
        m_GraphicsPipeline = new GraphicsPipeline("resource/Shader/shaderVert.spv",
                                                  "resource/Shader/shaderFrag.spv",
                                                  swapChain->getSwapChainExtent(),
                                                  swapChain->getRenderPass());
    }

    void VulkanRenderer::shutDown() {

    }

    void VulkanRenderer::beginRenderPass(VkCommandBuffer commandBuffer) {
        SwapChain* swapChain = Application::Get().getWindow().getSwapChain();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.66f, 0.9f, 0.96f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->getRenderPass();
        renderPassInfo.framebuffer = swapChain->getCurrentFrameBuffer();

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChain->getSwapChainExtent();
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                          *m_GraphicsPipeline->getPipeline());
    }

    void VulkanRenderer::endRenderPass(VkCommandBuffer commandBuffer) {
        vkCmdEndRenderPass(commandBuffer);
    }
}
