#pragma once

#include "Stellar/Core/Core.h"

#include "Stellar/Platform/Vulkan/Shader/VulkanShader.h"

#include "Stellar/Renderer/Pipeline.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Stellar {
	class VulkanPipeline : public Pipeline {
	public:
		VulkanPipeline(STLR_Ptr<Shader> shader, VkRenderPass renderPass);
		VulkanPipeline(const PipelineSpecification& spec);
		virtual ~VulkanPipeline();

		virtual PipelineSpecification& getSpecification() override { return m_Specification; };
		virtual const PipelineSpecification& getSpecification() const override { return m_Specification; };

		virtual void invalidate() override;
		virtual void setUniformBuffer(STLR_Ptr<Buffer> uniformBuffer, uint32_t binding, uint32_t set = 0) override;

		virtual void bind() override;
		
		VkPipeline getPipeline();
		VkPipelineLayout getPipelineLayout();
		std::vector<VkDescriptorSetLayout> getDescriptorSetLayout() { return m_DescriptorSetLayouts; }
		VkDescriptorSetLayout getUboSetLayout() { return m_UboSetLayout; }
		VkDescriptorSetLayout getTextureSetLayout() { return m_TextureSetLayout; }
		VkDescriptorPool getDescriptorPool() { return m_DescriptorPool; }
	private:
		void createDescriptorSetLayout();
		void createDescriptorPool();
	private:
		PipelineSpecification m_Specification;

		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkPipeline m_Pipeline = VK_NULL_HANDLE;
		VkPipelineCache m_PipelineCache = VK_NULL_HANDLE;

		VulkanShader::ShaderMaterialDescriptorSet m_DescriptorSets;

		std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

		VkDescriptorSetLayout m_UboSetLayout = VK_NULL_HANDLE;
		VkDescriptorSetLayout m_TextureSetLayout = VK_NULL_HANDLE;
	};
}