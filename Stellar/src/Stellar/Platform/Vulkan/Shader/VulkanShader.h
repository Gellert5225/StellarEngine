#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Shader.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Stellar {
	class STLR_API VulkanShader : public Shader {
	public:
		explicit VulkanShader(const std::string& filePath);
		~VulkanShader();

		VkShaderModule getShaderModule(const std::string& filePath);
		const std::vector<VkPipelineShaderStageCreateInfo>& getStageInfos() const;
	private:
		const std::string extractType(const std::string& filePath) const;
		void reflectAllStages(const std::unordered_map<Stellar::ShaderType, std::vector<uint32_t>>& spvShader);
		void reflect(VkShaderStageFlagBits shaderStage, const std::vector<uint32_t>& data);

		static VkShaderModule CreateShaderModule(const std::vector<uint32_t>& code);
	private:
		std::vector<VkPipelineShaderStageCreateInfo> m_StageInfos;
		std::vector<VkShaderModule> m_ShaderModules;
	};
}