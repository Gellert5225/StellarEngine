#include "stlrpch.h"
#include "VulkanRenderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "Stellar/Log.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

#include "Stellar/Platform/Vulkan/VulkanCommon.h"

namespace Stellar {

    struct VulkanRendererData {
        VkDescriptorPool descriptorPool;
        VkDescriptorSetLayout descriptorSetLayout;
        std::vector<VkDescriptorSet> descriptorSets;
    };

    static VulkanRendererData* s_Data = nullptr;

    void VulkanRenderer::init() {
        m_CommandBuffer = CommandBuffer::Create(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
        m_UniformBuffer = Buffer::Create(BufferType::Uniform, sizeof(GlobalUniforms));

        auto shaderLib = Renderer::GetShaderLibrary();
        auto shader = Renderer::GetShaderLibrary()->get("shader");
        m_GraphicsPipeline = new GraphicsPipeline(shader);
        s_Data = new VulkanRendererData();
        createDescriptorSets();
    }

    void VulkanRenderer::shutDown() {
        delete m_GraphicsPipeline;
        delete m_UniformBuffer;
        delete m_CommandBuffer;
    }

    void VulkanRenderer::beginRenderPass() {
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

        vkCmdBindPipeline((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer(),
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          *m_GraphicsPipeline->getPipeline());
    }

    void VulkanRenderer::endRenderPass() {
        vkCmdEndRenderPass((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer());
    }

    void VulkanRenderer::renderGeometry(Buffer* vertexBuffer,
                                        Buffer* indexBuffer,
                                        uint32_t indexCount,
                                        const glm::mat4& transform) {
        Push push{};
        push.model = transform;

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
        vkCmdBindDescriptorSets((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer(),
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                *m_GraphicsPipeline->getPipelineLayout(),
                                0, 1, &m_DescriptorSets[Renderer::GetCurrentFrameIndex()], 0, nullptr);
        vkCmdDrawIndexed((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer(),
                         indexCount, 1, 0, 0, 0);
    }

    void VulkanRenderer::setClearColor(const glm::vec4 &color) {
        m_ClearColor = {{ color.r, color.g, color.b, color.a }};
    }

    void VulkanRenderer::beginScene(Camera camera) {
        GlobalUniforms ubo{};

        ubo.viewProjection = camera.getViewProjectionMatrix();

        void* data;
        m_UniformBuffer->map(&data);
        m_UniformBuffer->write(data, &ubo);
        m_UniformBuffer->unMap();

        m_CommandBuffer->begin();
    }

    void VulkanRenderer::createDescriptorSets() {
        std::vector<VkDescriptorSetLayout> layouts(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT,
                                                   m_GraphicsPipeline->getDescriptorSetLayout());

        s_Data->descriptorPool = m_GraphicsPipeline->getDescriptorPool();
        s_Data->descriptorSetLayout = m_GraphicsPipeline->getDescriptorSetLayout();

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = m_GraphicsPipeline->getDescriptorPool();
        allocInfo.descriptorSetCount = VulkanSwapChain::MAX_FRAMES_IN_FLIGHT;
        allocInfo.pSetLayouts = layouts.data();

        m_DescriptorSets.resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);

        if (vkAllocateDescriptorSets(VulkanDevice::GetInstance()->logicalDevice(),
                                     &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        s_Data->descriptorSets = m_DescriptorSets;

        for (size_t i = 0; i < VulkanSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = (VkBuffer)m_UniformBuffer->getBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(GlobalUniforms);

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = m_DescriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0;
            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;
            descriptorWrite.pBufferInfo = &bufferInfo;

            vkUpdateDescriptorSets(VulkanDevice::GetInstance()->logicalDevice(), 1, &descriptorWrite, 0, nullptr);
        }

    }

     VkDescriptorSet VulkanRenderer::AllocateDescriptorSets(VkDescriptorSetAllocateInfo& info) {
        info.descriptorPool = s_Data->descriptorPool;
        info.pSetLayouts = &s_Data->descriptorSetLayout;
        VkDescriptorSet result;
        VK_CHECK_RESULT(vkAllocateDescriptorSets(VulkanDevice::GetInstance()->logicalDevice(),
                                     &info, &result));
        return result;
     }

     std::vector<VkDescriptorSet>& VulkanRenderer::GetDescriptorSets() {
        return s_Data->descriptorSets;
     }

    void VulkanRenderer::endScene() {
        m_CommandBuffer->end();
        m_CommandBuffer->submit();
    }
}
