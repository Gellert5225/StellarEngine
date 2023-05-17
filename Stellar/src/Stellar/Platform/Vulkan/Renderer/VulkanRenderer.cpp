#include "stlrpch.h"
#include "VulkanRenderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "Stellar/Core/Log.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"
#include "Stellar/Platform/Vulkan/Texture/VulkanTexture.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanUniformBuffer.h"
#include "Stellar/Platform/Vulkan/Material/VulkanMaterial.h"

namespace Stellar {

	struct VulkanRendererData {
		std::vector<VkDescriptorPool> DescriptorPools;

		// UniformBufferSet -> Shader Hash -> Frame -> WriteDescriptor
		std::unordered_map<UniformBufferSet*, std::unordered_map<uint64_t, std::vector<std::vector<VkWriteDescriptorSet>>>> uniformBufferWriteDescriptorCache;
	};

	static VulkanRendererData* s_Data = nullptr;

	static const std::vector<std::vector<VkWriteDescriptorSet>>& RetrieveOrCreateWriteDescriptors(STLR_Ptr<UniformBufferSet> uniformBufferSet, STLR_Ptr<VulkanMaterial> vulkanMaterial) {
		size_t shaderHash = vulkanMaterial->getShader()->getHash();
		if (s_Data->uniformBufferWriteDescriptorCache.find(uniformBufferSet.raw()) != s_Data->uniformBufferWriteDescriptorCache.end()) {
			const auto& shaderMap = s_Data->uniformBufferWriteDescriptorCache.at(uniformBufferSet.raw());
			if (shaderMap.find(shaderHash) != shaderMap.end()) {
				const auto& writeDescriptors = shaderMap.at(shaderHash);
				return writeDescriptors;
			}
		}

		uint32_t framesInFlight = Renderer::MAX_FRAMES_IN_FLIGHT;
		STLR_Ptr<VulkanShader> vulkanShader = vulkanMaterial->getShader().As<VulkanShader>();
		if (vulkanShader->hasDescriptorSet(0)) {
			const auto& shaderDescriptorSets = vulkanShader->getShaderDescriptorSets();
			if (!shaderDescriptorSets.empty()) {
				for (auto&& [binding, shaderUB] : shaderDescriptorSets[0].uniformBuffers) {
					auto& writeDescriptors = s_Data->uniformBufferWriteDescriptorCache[uniformBufferSet.raw()][shaderHash];
					writeDescriptors.resize(framesInFlight);
					for (uint32_t frame = 0; frame < framesInFlight; frame++) {
						STLR_Ptr<VulkanUniformBuffer> uniformBuffer = uniformBufferSet->get(binding, 0, frame); // set = 0 for now

						VkWriteDescriptorSet writeDescriptorSet = {};
						writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
						writeDescriptorSet.descriptorCount = 1;
						writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						writeDescriptorSet.pBufferInfo = &uniformBuffer->getDescriptorBufferInfo();
						writeDescriptorSet.dstBinding = uniformBuffer->getBinding();
						writeDescriptors[frame].push_back(writeDescriptorSet);
					}
				}
			
			}
		}

		return s_Data->uniformBufferWriteDescriptorCache[uniformBufferSet.raw()][shaderHash];
	}

	void VulkanRenderer::init() {
		s_Data = new VulkanRendererData();

		// descriptor pool
		s_Data->DescriptorPools.resize(Renderer::MAX_FRAMES_IN_FLIGHT);
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
		for (uint32_t i = 0; i < Renderer::MAX_FRAMES_IN_FLIGHT; i++) {
			VK_CHECK_RESULT(vkCreateDescriptorPool(device, &pool_info, nullptr, &s_Data->DescriptorPools[i]));
		}

		//createUboDescriptorSet();
	}

	void VulkanRenderer::shutDown() {
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		for (uint32_t i = 0; i < Renderer::MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroyDescriptorPool(device, s_Data->DescriptorPools[i], nullptr);
		}
	}


	void VulkanRenderer::beginRenderPass(STLR_Ptr<CommandBuffer> commandBuffer, 
										STLR_Ptr<RenderPass> renderPass, 
										bool explicitClear) {
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		auto swapChain = (VulkanSwapChain*)Application::Get().getWindow().getSwapChain();
		uint32_t bufferIndex = swapChain->getCurrentFrameIndex();
		VkCommandBuffer cmdBuffer = commandBuffer.As<VulkanCommandBuffer>()->getCurrentCommandBuffer(bufferIndex);

		auto fb = renderPass->getSpecification().targetFramebuffer;
		m_FrameBuffer = fb;

		if (m_NeedResize) {
			m_NeedResize = false;

			fb->resize(m_ViewPortWidth, m_ViewPortHeight);
		}

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = m_ClearColor;
		clearValues[1].depthStencil = {1.0f, 0};

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = ((VulkanFrameBuffer*)fb.raw())->getRenderPass();
		renderPassInfo.framebuffer = ((VulkanFrameBuffer*)fb.raw())->getFramebuffer();
		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent.width = fb->getSpecification().width;
		renderPassInfo.renderArea.extent.height = fb->getSpecification().height;
		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(fb->getSpecification().height);
		viewport.width = static_cast<float>(fb->getSpecification().width);
		viewport.height = -static_cast<float>(fb->getSpecification().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = {0, 0};
		scissor.extent.width = fb->getSpecification().width;
		scissor.extent.height = fb->getSpecification().height;
		vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
	}

	void VulkanRenderer::endRenderPass(STLR_Ptr<CommandBuffer> commandBuffer) {
		//vkResetDescriptorPool(VulkanDevice::GetInstance()->logicalDevice(), m_GraphicsPipeline->getDescriptorPool(), 0);
		auto swapChain = (VulkanSwapChain*)Application::Get().getWindow().getSwapChain();
		uint32_t bufferIndex = swapChain->getCurrentFrameIndex();
		VkCommandBuffer cmdBuffer = commandBuffer.As<VulkanCommandBuffer>()->getCurrentCommandBuffer(bufferIndex);
		vkCmdEndRenderPass(cmdBuffer);
		// commandBuffer->end();
		// commandBuffer->submit();
	}

	void VulkanRenderer::beginFrame() {
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		auto swapChain = (VulkanSwapChain*)Application::Get().getWindow().getSwapChain();
		uint32_t bufferIndex = swapChain->getCurrentFrameIndex();
		vkResetDescriptorPool(device, s_Data->DescriptorPools[bufferIndex], 0);
	}

	void VulkanRenderer::renderGeometry(STLR_Ptr<CommandBuffer> renderCommandBuffer, 
							STLR_Ptr<Pipeline> pipeline,
							STLR_Ptr<UniformBufferSet> uniformBufferSet, 
							STLR_Ptr<Material> material, 
							STLR_Ptr<Buffer> vertexBuffer, 
							STLR_Ptr<Buffer> indexBuffer, 
							const glm::mat4& transform, 
							uint32_t indexCount) {
		Push push{};
		push.model = transform;
		push.screenSize = {m_FrameBuffer->getSpecification().width, m_FrameBuffer->getSpecification().height};

		auto vulkanMaterial = material.As<VulkanMaterial>();
		uint32_t frameIndex = Renderer::GetCurrentFrameIndex();

		VkCommandBuffer commandBuffer = renderCommandBuffer.As<VulkanCommandBuffer>()->getCurrentCommandBuffer(frameIndex);
		
		auto vulkanPipeline = pipeline.As<VulkanPipeline>();
		VkPipeline vkPipeline = vulkanPipeline->getPipeline();
		VkPipelineLayout layout = vulkanPipeline->getPipelineLayout();

		VkDeviceSize offsets[1] = {0};
		auto buffers = (VkBuffer)vertexBuffer->getBuffer();
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, (VkBuffer)indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline);

		const auto& writeDescriptors = RetrieveOrCreateWriteDescriptors(uniformBufferSet, vulkanMaterial);
		vulkanMaterial->update(writeDescriptors);

		uint32_t bufferIndex = Renderer::GetCurrentFrameIndex();
		VkDescriptorSet descriptorSet = vulkanMaterial->getDescriptorSet(bufferIndex);
		if (descriptorSet)
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0, 1, &descriptorSet, 0, nullptr);

		vkCmdPushConstants(commandBuffer, layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Push), &push);
		vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
	}

	void VulkanRenderer::renderGrid(STLR_Ptr<Buffer> vertexBuffer, STLR_Ptr<Buffer> indexBuffer, uint32_t indexCount) {
		// auto commandBuffer = (VkCommandBuffer)m_CommandBuffer->getActiveCommandBuffer();

		// vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GridPipeline->getPipeline());

		// VkDeviceSize offsets[] = {0};
		// auto buffers = (VkBuffer)vertexBuffer->getBuffer();

		// vkCmdBindVertexBuffers(commandBuffer, 0, 1, &buffers, offsets);
		// vkCmdBindIndexBuffer(commandBuffer, (VkBuffer)indexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);
		// vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
	}

	void VulkanRenderer::setClearColor(const glm::vec4 &color) {
		m_ClearColor = {{ color.r, color.g, color.b, color.a }};
	}

	void VulkanRenderer::bindUbo(const GlobalUniforms& ubo) {
		//m_UniformBuffer->setData(&ubo, sizeof(GlobalUniforms));
	}

	STLR_Ptr<FrameBuffer> VulkanRenderer::getFrameBuffer() {
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
