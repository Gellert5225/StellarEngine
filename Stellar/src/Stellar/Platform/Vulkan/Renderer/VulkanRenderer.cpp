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
		std::vector<VkDescriptorPool> DescriptorPools;
	};

	static VulkanRendererData* s_Data = nullptr;

	void VulkanRenderer::init() {
		m_CommandBuffer = CommandBuffer::Create(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
		m_UniformBuffer = Buffer::Create(BufferType::Uniform, sizeof(GlobalUniforms));

		FrameBufferSpec framebufferSpec;
		framebufferSpec.width = 1280;
		framebufferSpec.height = 720;
		framebufferSpec.attachments = { ImageFormat::RGBA32F, ImageFormat::Depth };
		m_FrameBuffer = FrameBuffer::Create(framebufferSpec);

		auto quadShader = Renderer::GetShaderLibrary()->get("shader");
		auto gridShader = Renderer::GetShaderLibrary()->get("grid");
		m_GraphicsPipeline = new GraphicsPipeline(quadShader, ((VulkanFrameBuffer*)m_FrameBuffer.get())->getRenderPass());
		m_GridPipeline = new GraphicsPipeline(gridShader, ((VulkanFrameBuffer*)m_FrameBuffer.get())->getRenderPass());
		s_Data = new VulkanRendererData();
		s_Data->pipeline = m_GraphicsPipeline;

		// descriptor pool
		s_Data->DescriptorPools.resize(VulkanSwapChain::MAX_FRAMES_IN_FLIGHT);
		VkDescriptorPoolSize pool_sizes[] = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};
		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets = 100000;
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		for (uint32_t i = 0; i < VulkanSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
			VK_CHECK_RESULT(vkCreateDescriptorPool(device, &pool_info, nullptr, &s_Data->DescriptorPools[i]));
		}

		createUboDescriptorSet();
	}

	void VulkanRenderer::shutDown() {
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		for (uint32_t i = 0; i < VulkanSwapChain::MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroyDescriptorPool(device, s_Data->DescriptorPools[i], nullptr);
		}
		delete s_Data->pipeline;
		delete m_GridPipeline;
		delete m_UniformBuffer;
		delete m_CommandBuffer;
	}

	void VulkanRenderer::beginRenderPass() {
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		auto swapChain = (VulkanSwapChain*)Application::Get().getWindow().getSwapChain();
		uint32_t bufferIndex = swapChain->getCurrentFrameIndex();
		vkResetDescriptorPool(device, s_Data->DescriptorPools[bufferIndex], 0);

		if (m_NeedResize) {
			m_NeedResize = false;

			m_FrameBuffer->resize(m_ViewPortWidth, m_ViewPortHeight);
		}

		m_CommandBuffer->begin();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = m_ClearColor;
		clearValues[1].depthStencil = {1.0f, 0};

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = ((VulkanFrameBuffer*)m_FrameBuffer.get())->getRenderPass();
		renderPassInfo.framebuffer = ((VulkanFrameBuffer*)m_FrameBuffer.get())->getFramebuffer();
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent.width = m_FrameBuffer->getSpecification().width;
		renderPassInfo.renderArea.extent.height = m_FrameBuffer->getSpecification().height;
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues.data();

		auto commandBuffer = (VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer();
		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(m_FrameBuffer->getSpecification().height);
		viewport.width = static_cast<float>(m_FrameBuffer->getSpecification().width);
		viewport.height = -static_cast<float>(m_FrameBuffer->getSpecification().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent.width = m_FrameBuffer->getSpecification().width;
		scissor.extent.height = m_FrameBuffer->getSpecification().height;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
								m_GraphicsPipeline->getPipelineLayout(),
								1, 1, &m_UboDescriptorSet, 0, nullptr);
	}

	void VulkanRenderer::endRenderPass() {
		//vkResetDescriptorPool(VulkanDevice::GetInstance()->logicalDevice(), m_GraphicsPipeline->getDescriptorPool(), 0);
		vkCmdEndRenderPass((VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer());
		m_CommandBuffer->end();
		m_CommandBuffer->submit();
	}

	void VulkanRenderer::renderGeometry(Buffer* vertexBuffer,
										Buffer* indexBuffer,
										Ref<Texture2D> texture,
										const glm::vec4& color,
										uint32_t indexCount,
										const glm::mat4& transform) {
		Push push{};
		push.model = transform;
		push.color = color;

		if (!texture) {
			uint32_t whiteTextureData = 0xffffffff;
			texture = Texture2D::Create(ImageFormat::RGBA, 1, 1, &whiteTextureData);
		}

		auto textureDescriptorSet = ((VulkanTexture*)texture.get())->getDescriptorSets();
		auto commandBuffer = (VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer();

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->getPipeline());
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
								m_GraphicsPipeline->getPipelineLayout(),
								0, 1, &textureDescriptorSet, 0, nullptr);
		vkCmdPushConstants(commandBuffer, m_GraphicsPipeline->getPipelineLayout(),
						VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
						0, sizeof(Push), &push);

		VkDeviceSize offsets[] = {0};
		auto buffers = (VkBuffer)vertexBuffer->getBuffer();

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, (VkBuffer)indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
	}

	void VulkanRenderer::renderGrid(Buffer* vertexBuffer, Buffer* indexBuffer, uint32_t indexCount) {
		auto commandBuffer = (VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer();

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GridPipeline->getPipeline());

		VkDeviceSize offsets[] = {0};
		auto buffers = (VkBuffer)vertexBuffer->getBuffer();

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, (VkBuffer)indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
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

	void VulkanRenderer::createUboDescriptorSet() {
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		auto uboSetLayout = m_GraphicsPipeline->getUboSetLayout();

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_GraphicsPipeline->getDescriptorPool();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &uboSetLayout;

		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &m_UboDescriptorSet));

		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = (VkBuffer)m_UniformBuffer->getBuffer();
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(GlobalUniforms);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_UboDescriptorSet;
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

	Ref<FrameBuffer> VulkanRenderer::getFrameBuffer() {
		return m_FrameBuffer;
	}

	void VulkanRenderer::resizeFrameBuffer(uint32_t width, uint32_t height) {
		if (m_ViewPortWidth != width || m_ViewPortHeight != height) {
			m_ViewPortWidth = width;
			m_ViewPortHeight = height;
			m_NeedResize = true;
		}
	}

	VkDescriptorSet VulkanRenderer::AllocateDesriptorSet(VkDescriptorSetAllocateInfo& allocInfo) {
		uint32_t bufferIndex = Renderer::GetCurrentFrameIndex();
		allocInfo.descriptorPool = s_Data->DescriptorPools[bufferIndex];
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		VkDescriptorSet result;
		VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &result));
		return result;
	}

	VkDescriptorPool VulkanRenderer::GetDescriptorPool() {
		return s_Data->DescriptorPools[Renderer::GetCurrentFrameIndex()];
	}
}
