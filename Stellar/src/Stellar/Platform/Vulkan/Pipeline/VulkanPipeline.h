#pragma once

#include "Stellar/Core/Core.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Stellar {
	class STLR_API VulkanPipeline {
	public:
		VulkanPipeline() = default;
		virtual ~VulkanPipeline() = 0;

		VkPipeline getPipeline();
		VkPipelineLayout getPipelineLayout();
		std::vector<VkDescriptorSetLayout> getDescriptorSetLayout() { return m_DescriptorSetLayouts; }
		VkDescriptorSetLayout getUboSetLayout() { return m_UboSetLayout; }
		VkDescriptorSetLayout getTextureSetLayout() { return m_TextureSetLayout; }
		VkDescriptorPool getDescriptorPool() { return m_DescriptorPool; }

	protected:
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_Pipeline = VK_NULL_HANDLE;

		std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

		VkDescriptorSetLayout m_UboSetLayout = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_TextureSetLayout = VK_NULL_HANDLE;

		virtual void createDescriptorSetLayout() = 0;
		virtual void createDescriptorPool() = 0;
	};
}