#include "stlrpch.h"
#include "VulkanPipeline.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

#include "Stellar/Core/Log.h"

namespace Stellar {
	VulkanPipeline::~VulkanPipeline() {
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		vkDestroyPipeline(device, m_Pipeline, nullptr);
		vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);

		vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);
		for (auto setLayout : m_DescriptorSetLayouts) {
			vkDestroyDescriptorSetLayout(device, setLayout, nullptr);
		}
	}

	VkPipeline VulkanPipeline::getPipeline() {
		return m_Pipeline;
	}

	VkPipelineLayout VulkanPipeline::getPipelineLayout() {
		return m_PipelineLayout;
	}
}