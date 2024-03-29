#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Shader.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Stellar {
	class VulkanShader : public Shader {
	public:
		struct UniformBuffer {
			VkDescriptorBufferInfo descriptor;
			uint32_t size = 0;
			uint32_t bindingPoint = 0;
			std::string name;
			VkShaderStageFlagBits shaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		};

		struct ImageSampler {
			uint32_t bindingPoint = 0;
			uint32_t descriptorSet = 0;
			uint32_t arraySize = 0;
			std::string name;
			VkShaderStageFlagBits shaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		};

		struct PushConstantRange {
			VkShaderStageFlagBits ShaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
			uint32_t Offset = 0;
			uint32_t Size = 0;
		};

		struct ShaderDescriptorSet {
			std::unordered_map<uint32_t, UniformBuffer*> uniformBuffers;
			std::unordered_map<uint32_t, ImageSampler> imageSamplers;
			std::unordered_map<uint32_t, ImageSampler> storageImages;
			std::unordered_map<uint32_t, ImageSampler> separateSamplers;
			std::unordered_map<uint32_t, ImageSampler> separateImages;

			std::unordered_map<std::string, VkWriteDescriptorSet> writeDescriptorSets;

			operator bool() const { return !(uniformBuffers.empty() && imageSamplers.empty() && storageImages.empty()); }
		};

		struct ShaderMaterialDescriptorSet {
			VkDescriptorPool Pool = nullptr;
			std::vector<VkDescriptorSet> DescriptorSets;
		};
	public:
		explicit VulkanShader(const std::string& filePath);
		~VulkanShader();

		virtual const std::unordered_map<std::string, ShaderResourceDeclaration>& getResources() const override;

		VkShaderModule getShaderModule(const std::string& filePath);
		std::vector<VkDescriptorSetLayout> getAllDescriptorSetLayouts();
		const std::vector<VkPipelineShaderStageCreateInfo>& getStageInfos() const;
		const std::vector<PushConstantRange>& getPushConstantRanges() const { return m_PushConstantRanges; }
		const std::vector<ShaderDescriptorSet>& getShaderDescriptorSets() const { return m_ShaderDescriptorSets; }
		const VkWriteDescriptorSet* getDescriptorSet(const std::string& name, uint32_t set = 0);
		ShaderMaterialDescriptorSet allocateDescriptorSet(uint32_t set = 0);
		bool hasDescriptorSet(uint32_t set) const { return m_TypeCounts.find(set) != m_TypeCounts.end(); }
	private:
		const std::string extractType(const std::string& filePath) const;
		void reflectAllStages(const std::unordered_map<Stellar::ShaderType, std::vector<uint32_t>>& spvShader);
		void reflect(VkShaderStageFlagBits shaderStage, const std::vector<uint32_t>& data);
		void createDescriptors();

		static VkShaderModule CreateShaderModule(const std::vector<uint32_t>& code);
	private:
		std::vector<VkPipelineShaderStageCreateInfo> m_StageInfos;
		std::vector<VkShaderModule> m_ShaderModules;

		std::vector<ShaderDescriptorSet> m_ShaderDescriptorSets;
		std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;

		std::vector<PushConstantRange> m_PushConstantRanges;
		std::unordered_map<std::string, ShaderBuffer> m_Buffers;
		std::unordered_map<std::string, ShaderResourceDeclaration> m_Resources;
		std::unordered_map<uint32_t, std::vector<VkDescriptorPoolSize>> m_TypeCounts;
	};
}