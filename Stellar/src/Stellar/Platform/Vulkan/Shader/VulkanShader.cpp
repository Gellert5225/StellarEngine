#include "stlrpch.h"
#include "VulkanShader.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/Shader/VulkanShaderCompiler.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"

#include "Stellar/Core/Log.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Stellar {

	VulkanShader::VulkanShader(const std::string& filePath) : Shader(filePath) {
		auto source = ReadFile(filePath);
		STLR_CORE_INFO("Preproecssing Shader: {0}", m_Name);
		auto result = VulkanShaderCompiler::PreProcess(source);
		std::unordered_map<ShaderType, std::vector<uint32_t>> spv;

		STLR_CORE_INFO("Compiling Shader: {0}", m_Name);
		VulkanShaderCompiler::Compile(result, spv);

		for (auto& kv : spv) {
			auto shaderModule = CreateShaderModule(kv.second);
			m_ShaderModules.push_back(shaderModule);

			VkPipelineShaderStageCreateInfo shaderStageInfo{};
			shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageInfo.stage = VulkanShaderCompilerUtil::VulkanShaderTypeFromType(kv.first);
			shaderStageInfo.module = shaderModule;
			shaderStageInfo.pName = "main";
			m_StageInfos.push_back(shaderStageInfo);
		}
		reflectAllStages(spv);
	}
	void VulkanShader::reflectAllStages(const std::unordered_map<Stellar::ShaderType, std::vector<uint32_t>>& spvShader) {
		for (auto [stage, data] : spvShader)
			reflect(VulkanShaderCompilerUtil::VulkanShaderTypeFromType(stage), data);
	}

	void VulkanShader::reflect(VkShaderStageFlagBits shaderStage, const std::vector<uint32_t>& data) {
		VkDevice device = VulkanDevice::GetInstance()->logicalDevice(); 

		STLR_CORE_TRACE("===========================");
		STLR_CORE_TRACE(" Vulkan Shader Reflection");
		STLR_CORE_TRACE(" {0}", m_FilePath);
		STLR_CORE_TRACE("===========================");

		spirv_cross::Compiler compiler(data);
		auto resources = compiler.get_shader_resources();

		STLR_CORE_TRACE("Uniform Buffers:");
		for (const auto& resource : resources.uniform_buffers) {
			const auto& name = resource.name;
			auto& bufferType = compiler.get_type(resource.base_type_id);
			int memberCount = bufferType.member_types.size();
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			uint32_t size = compiler.get_declared_struct_size(bufferType);

			ShaderDescriptorSet& shaderDescriptorSet = m_ShaderDescriptorSets[descriptorSet];
			//HZ_CORE_ASSERT(shaderDescriptorSet.UniformBuffers.find(binding) == shaderDescriptorSet.UniformBuffers.end());
			if (s_UniformBuffers[descriptorSet].find(binding) == s_UniformBuffers[descriptorSet].end())
			{
				UniformBuffer* uniformBuffer = new UniformBuffer();
				uniformBuffer->BindingPoint = binding;
				uniformBuffer->Size = size;
				uniformBuffer->Name = name;
				uniformBuffer->ShaderStage = shaderStage;
				s_UniformBuffers.at(descriptorSet)[binding] = uniformBuffer;

				AllocateUniformBuffer(*uniformBuffer);
			}
			else
			{
				UniformBuffer* uniformBuffer = s_UniformBuffers.at(descriptorSet).at(binding);
				if (size > uniformBuffer->Size)
				{
					STLR_CORE_TRACE("Resizing uniform buffer (binding = {0}, set = {1}) to {2} bytes", binding, descriptorSet, size);
					uniformBuffer->Size = size;
					AllocateUniformBuffer(*uniformBuffer);
				}
				
			}

			shaderDescriptorSet.UniformBuffers[binding] = s_UniformBuffers.at(descriptorSet).at(binding);

			STLR_CORE_TRACE("  {0} ({1}, {2})", name, descriptorSet, binding);
			STLR_CORE_TRACE("  Member Count: {0}", memberCount);
			STLR_CORE_TRACE("  Size: {0}", size);
			STLR_CORE_TRACE("-------------------");
		}
	}

	const std::string VulkanShader::extractType(const std::string& filePath) const {
		auto fileName = filePath.substr(filePath.find_last_of("/") + 1);
		return fileName.substr(fileName.find_first_of(".") + 1, 4);
	}

	VkShaderModule VulkanShader::CreateShaderModule(const std::vector<uint32_t>& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size() * sizeof(uint32_t);
		createInfo.pCode = code.data();

		VkShaderModule shaderModule;
		auto device = VulkanDevice::GetInstance()->logicalDevice();

		VK_CHECK_RESULT(vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule));
		return shaderModule;
	}

	VulkanShader::~VulkanShader() {
		for (auto shader : m_ShaderModules) 
			vkDestroyShaderModule(VulkanDevice::GetInstance()->logicalDevice(),
				shader, nullptr);
	}

	const std::vector<VkPipelineShaderStageCreateInfo>& VulkanShader::getStageInfos() const {
		return m_StageInfos;
	}
}