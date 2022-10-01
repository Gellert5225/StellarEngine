#include "stlrpch.h"
#include "VulkanRenderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "Stellar/Core/Log.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"
#include "Stellar/Platform/Vulkan/Texture/VulkanTexture.h"

namespace Stellar {

    struct VulkanRendererData {
        GraphicsPipeline* pipeline;
    };

    static VulkanRendererData* s_Data = nullptr;

    void VulkanRenderer::init() {
        m_CommandBuffer = CommandBuffer::Create(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
        m_UniformBuffer = Buffer::Create(BufferType::Uniform, sizeof(GlobalUniforms));

        auto shader = Renderer::GetShaderLibrary()->get("shader");
        m_GraphicsPipeline = new GraphicsPipeline(shader);
        s_Data = new VulkanRendererData();
        s_Data->pipeline = m_GraphicsPipeline;
        createDescriptorSets();
    }

    void VulkanRenderer::shutDown() {
        delete m_GraphicsPipeline;
        delete m_UniformBuffer;
        delete m_CommandBuffer;
    }

    void VulkanRenderer::beginRenderPass() {
        m_CommandBuffer->begin();

        auto swapChain = (VulkanSwapChain*)Application::Get().getWindow().getSwapChain();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = m_ClearColor;
        clearValues[1].depthStencil = {1.0f, 0};

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->getRenderPass();
        renderPassInfo.framebuffer = swapChain->getCurrentFrameBuffer();
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent.width = swapChain->getSwapChainExtent().width;
        renderPassInfo.renderArea.extent.height = swapChain->getSwapChainExtent().height;
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer(),
                             &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer(), 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent.width = swapChain->getSwapChainExtent().width;
        scissor.extent.height = swapChain->getSwapChainExtent().height;
        vkCmdSetScissor((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer(), 0, 1, &scissor);
    }

    void VulkanRenderer::endRenderPass() {
        vkCmdEndRenderPass((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer());
        m_CommandBuffer->end();
        m_CommandBuffer->submit();
    }

    void VulkanRenderer::renderGeometry(Buffer* vertexBuffer,
                                        Buffer* indexBuffer,
                                        Texture2D* texture,
                                        const glm::vec3& color,
                                        uint32_t indexCount,
                                        const glm::mat4& transform) {
        Push push{};
        push.model = transform;
        push.color = color;

        auto descriptorSet = ((VulkanTexture*)texture)->getDescriptorSets();
        vkCmdBindPipeline((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer(),
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          *m_GraphicsPipeline->getPipeline());
        vkCmdBindDescriptorSets((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer(),
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                *m_GraphicsPipeline->getPipelineLayout(),
                                0, 1, 
                                &descriptorSet,
                                0, nullptr);
        
        vkCmdPushConstants((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer(),
                           *m_GraphicsPipeline->getPipelineLayout(),
                           VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                           0,
                           sizeof(Push),
                           &push);

        VkDeviceSize offsets[] = {0};
        auto buffers = (VkBuffer)vertexBuffer->getBuffer();
        vkCmdBindVertexBuffers((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer(),
                               0, 1, &buffers, offsets);
        vkCmdBindIndexBuffer((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer(),
                             (VkBuffer)indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
        vkCmdDrawIndexed((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer(),
                         indexCount, 1, 0, 0, 0);
    }

    void VulkanRenderer::setClearColor(const glm::vec4 &color) {
        m_ClearColor = {{ color.r, color.g, color.b, color.a }};
    }

    void VulkanRenderer::bindUbo(const GlobalUniforms& ubo) {
        void* data;
        m_UniformBuffer->map(&data);
        m_UniformBuffer->write(data, &ubo);
        m_UniformBuffer->unMap();
    }

    void VulkanRenderer::createDescriptorSets() {
        auto device = VulkanDevice::GetInstance()->logicalDevice();
        auto uboSetLayout = m_GraphicsPipeline->getUboSetLayout();

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_GraphicsPipeline->getDescriptorPool();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &uboSetLayout;

        VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &m_DescriptorSet));

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = (VkBuffer)m_UniformBuffer->getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(GlobalUniforms);

        VkWriteDescriptorSet descriptorWrite;
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_DescriptorSet;
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
    }

    GraphicsPipeline* VulkanRenderer::GetPipeline() {
        return s_Data->pipeline;
    }
}
