#include "stlrpch.h"
#include "VulkanRenderer.h"

#include "Stellar/Log.h"
#include "Stellar/Platform/Vulkan/Descriptor/DescriptorSetLayout.h"

#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

namespace Stellar {

    void VulkanRenderer::init() {
        m_UniformBuffer = Buffer::Create(BufferType::Uniform, sizeof(GlobalUniforms));
        auto* layout = new DescriptorSetLayout();

        SwapChain* swapChain = Application::Get().getWindow().getSwapChain();
        m_GraphicsPipeline = new GraphicsPipeline("resource/Shader/shaderVert.spv",
                                                  "resource/Shader/shaderFrag.spv",
                                                  swapChain->getRenderPass(),
                                                  layout->getDescriptorSetLayouts());
    }

    void VulkanRenderer::shutDown() {
        delete m_GraphicsPipeline;
        delete m_UniformBuffer;
    }

    void VulkanRenderer::beginRenderPass(CommandBuffer* commandBuffer) {
        SwapChain* swapChain = Application::Get().getWindow().getSwapChain();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = m_ClearColor;
        clearValues[1].depthStencil = {1.0f, 0};

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->getRenderPass();
        renderPassInfo.framebuffer = swapChain->getCurrentFrameBuffer();
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass((VkCommandBuffer)commandBuffer->getActiveCommandBuffer(),
                             &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport((VkCommandBuffer)commandBuffer->getActiveCommandBuffer(), 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = swapChain->getSwapChainExtent();
        vkCmdSetScissor((VkCommandBuffer)commandBuffer->getActiveCommandBuffer(), 0, 1, &scissor);

        vkCmdBindPipeline((VkCommandBuffer)commandBuffer->getActiveCommandBuffer(),
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          *m_GraphicsPipeline->getPipeline());
    }

    void VulkanRenderer::endRenderPass(CommandBuffer* commandBuffer) {
        vkCmdEndRenderPass((VkCommandBuffer)commandBuffer->getActiveCommandBuffer());
    }

    void VulkanRenderer::renderGeometry(CommandBuffer* commandBuffer,
                                        Buffer* vertexBuffer,
                                        Buffer* indexBuffer,
                                        uint32_t indexCount) {
        VkDeviceSize offsets[] = {0};
        auto buffers = (VkBuffer)vertexBuffer->getBuffer();
        vkCmdBindVertexBuffers((VkCommandBuffer)commandBuffer->getActiveCommandBuffer(),
                               0, 1, &buffers, offsets);
        vkCmdBindIndexBuffer((VkCommandBuffer)commandBuffer->getActiveCommandBuffer(),
                             (VkBuffer)indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed((VkCommandBuffer)commandBuffer->getActiveCommandBuffer(),
                         indexCount, 1, 0, 0, 0);
    }

    void VulkanRenderer::setClearColor(const glm::vec4 &color) {
        m_ClearColor = {{ color.r, color.g, color.b, color.a }};
    }

    void VulkanRenderer::beginScene() {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        auto extent = Application::Get().getWindow().getSwapChain()->getSwapChainExtent();
        GlobalUniforms ubo{};
        ubo.model = glm::rotate(glm::mat4(1.0f),
                                time * glm::radians(90.0f),
                                glm::vec3(0.0f, 0.0f, 1.0f));

        auto proj = glm::perspective(glm::radians(45.0f), extent.width / (float) extent.height, 0.1f, 10.0f);
        proj[1][1] *= -1;
        ubo.viewProjection = proj * glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
                                                glm::vec3(0.0f, 0.0f, 0.0f),
                                                glm::vec3(0.0f, 0.0f, 1.0f));
        void* data;
        m_UniformBuffer->map(&data);
        m_UniformBuffer->write(data, &ubo);
        m_UniformBuffer->unMap();
    }
}
