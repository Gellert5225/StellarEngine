#include "stlrpch.h"
#include "VulkanShader.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/Shader/VulkanShaderCompiler.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"
#include "Stellar/Platform/Vulkan/Renderer/VulkanRenderer.h"

#include "Stellar/Core/Log.h"

#include <shaderc/shaderc.hpp>
#include <spirv_cross/spirv_glsl.hpp>

namespace Stellar {

	static std::unordered_map<uint32_t, std::unordered_map<uint32_t, VulkanShader::UniformBuffer*>> s_UniformBuffers;

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

		createDescriptors();
	}

	void VulkanShader::reflectAllStages(const std::unordered_map<Stellar::ShaderType, std::vector<uint32_t>>& spvShader) {
		for (auto [stage, data] : spvShader)
			reflect(VulkanShaderCompilerUtil::VulkanShaderTypeFromType(stage), data);
	}

	void VulkanShader::reflect(VkShaderStageFlagBits shaderStage, const std::vector<uint32_t>& data) {
		VkDevice device = VulkanDevice::GetInstance()->logicalDevice(); 

		STLR_CORE_TRACE("===========================");
		STLR_CORE_TRACE("Vulkan Shader Reflection");
		STLR_CORE_TRACE("Shader: {0}", m_FilePath);
		STLR_CORE_TRACE("Stage: {0}", VulkanShaderCompilerUtil::GetVulkanShaderTypeString(shaderStage));
		STLR_CORE_TRACE("===========================");

		spirv_cross::Compiler compiler(std::move(data));
		auto resources = compiler.get_shader_resources();

		STLR_CORE_TRACE(" Push Constant Buffers:");
		for (const auto& resource : resources.push_constant_buffers) {
			const auto& bufferName = resource.name;
			auto& bufferType = compiler.get_type(resource.base_type_id);
			auto bufferSize = compiler.get_declared_struct_size(bufferType);
			int memberCount = bufferType.member_types.size();
			uint32_t bufferOffset = 0;
			if (m_PushConstantRanges.size())
				bufferOffset = m_PushConstantRanges.back().Offset + m_PushConstantRanges.back().Size;

			auto& pushConstantRange = m_PushConstantRanges.emplace_back();
			pushConstantRange.ShaderStage = shaderStage;
			pushConstantRange.Size = bufferSize;
			pushConstantRange.Offset = bufferOffset;

			// Skip empty push constant buffers - these are for the renderer only
			if (bufferName.empty() || bufferName == "u_Renderer")
				continue;

			ShaderBuffer& buffer = m_Buffers[bufferName];
			buffer.Name = bufferName;
			buffer.Size = bufferSize - bufferOffset;

			STLR_CORE_TRACE("    Name: {0}", bufferName);
			STLR_CORE_TRACE("    Member Count: {0}", memberCount);
			STLR_CORE_TRACE("    Size: {0}", bufferSize);
			STLR_CORE_TRACE("-------------------");

			for (int i = 0; i < memberCount; i++) {
				auto type = compiler.get_type(bufferType.member_types[i]);
				const auto& memberName = compiler.get_member_name(bufferType.self, i);
				auto size = compiler.get_declared_struct_member_size(bufferType, i);
				auto offset = compiler.type_struct_member_offset(bufferType, i) - bufferOffset;

				std::string uniformName = bufferName + "." + memberName;
				buffer.Uniforms[uniformName] = ShaderUniform(uniformName, VulkanShaderCompilerUtil::SPIRTypeToShaderUniformType(type), size, offset);
			}
		}

		STLR_CORE_TRACE(" Uniform Buffers:");
		for (const auto& resource : resources.uniform_buffers) {
			const auto& name = resource.name;
			auto& bufferType = compiler.get_type(resource.base_type_id);
			int memberCount = bufferType.member_types.size();
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			uint32_t size = compiler.get_declared_struct_size(bufferType);

			if (descriptorSet >= m_ShaderDescriptorSets.size())
				m_ShaderDescriptorSets.resize(descriptorSet + 1);

			ShaderDescriptorSet& shaderDescriptorSet = m_ShaderDescriptorSets[descriptorSet];
			if (s_UniformBuffers[descriptorSet].find(binding) == s_UniformBuffers[descriptorSet].end()) {
				UniformBuffer* uniformBuffer = new UniformBuffer();
				uniformBuffer->bindingPoint = binding;
				uniformBuffer->size = size;
				uniformBuffer->name = name;
				uniformBuffer->shaderStage = shaderStage;
				s_UniformBuffers.at(descriptorSet)[binding] = uniformBuffer;
			} else {
				UniformBuffer* uniformBuffer = s_UniformBuffers.at(descriptorSet).at(binding);
				if (size > uniformBuffer->size)
					uniformBuffer->size = size;
			}

			shaderDescriptorSet.uniformBuffers[binding] = s_UniformBuffers.at(descriptorSet).at(binding);

			STLR_CORE_TRACE("    Name: {0}", name);
			STLR_CORE_TRACE("    Descriptor Set: {0}, Binding: {1}",  descriptorSet, binding);
			STLR_CORE_TRACE("    Member Count: {0}", memberCount);
			STLR_CORE_TRACE("    Size: {0}", size);
			STLR_CORE_TRACE("-------------------");
		}

		STLR_CORE_TRACE(" Sampled Images:");
		for (const auto& resource : resources.sampled_images) {
			const auto& name = resource.name;
			auto& type = compiler.get_type(resource.base_type_id);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			uint32_t dimension = type.image.dim;

			if (descriptorSet >= m_ShaderDescriptorSets.size())
				m_ShaderDescriptorSets.resize(descriptorSet + 1);

			ShaderDescriptorSet& shaderDescriptorSet = m_ShaderDescriptorSets[descriptorSet];
			auto& imageSampler = shaderDescriptorSet.imageSamplers[binding];
			imageSampler.bindingPoint = binding;
			imageSampler.descriptorSet = descriptorSet;
			imageSampler.name = name;
			imageSampler.shaderStage = shaderStage;

			m_Resources[name] = ShaderResourceDeclaration(name, binding, 1);

			STLR_CORE_TRACE("    Name: {0}", name);
			STLR_CORE_TRACE("    Descriptor Set: {0}, Binding: {1}",  descriptorSet, binding);
			STLR_CORE_TRACE("-------------------");
		}

		STLR_CORE_TRACE(" Storage Images:");
		for (const auto& resource : resources.storage_images) {
			const auto& name = resource.name;
			auto& type = compiler.get_type(resource.base_type_id);
			uint32_t binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
			uint32_t descriptorSet = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
			uint32_t dimension = type.image.dim;

			if (descriptorSet >= m_ShaderDescriptorSets.size())
				m_ShaderDescriptorSets.resize(descriptorSet + 1);

			ShaderDescriptorSet& shaderDescriptorSet = m_ShaderDescriptorSets[descriptorSet];
			auto& imageSampler = shaderDescriptorSet.storageImages[binding];
			imageSampler.bindingPoint = binding;
			imageSampler.descriptorSet = descriptorSet;
			imageSampler.name = name;
			imageSampler.shaderStage = shaderStage;

			m_Resources[name] = ShaderResourceDeclaration(name, binding, 1);

			STLR_CORE_TRACE("    Name: {0}", name);
			STLR_CORE_TRACE("    Descriptor Set: {0}, Binding: {1}",  descriptorSet, binding);
		}

		STLR_CORE_TRACE("===========================");
	}

	void VulkanShader::createDescriptors() {
		VkDevice device = VulkanDevice::GetInstance()->logicalDevice();
		
		m_TypeCounts.clear();
		for (uint32_t set = 0; set < m_ShaderDescriptorSets.size(); set++) {
			auto& shaderDescriptorSet = m_ShaderDescriptorSets[set];

			if (shaderDescriptorSet.uniformBuffers.size()) {
				VkDescriptorPoolSize& typeCount = m_TypeCounts[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				typeCount.descriptorCount = shaderDescriptorSet.uniformBuffers.size();
			}
			if (shaderDescriptorSet.imageSamplers.size()) {
				VkDescriptorPoolSize& typeCount = m_TypeCounts[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				typeCount.descriptorCount = shaderDescriptorSet.imageSamplers.size();
			}
			if (shaderDescriptorSet.storageImages.size()) {
				VkDescriptorPoolSize& typeCount = m_TypeCounts[set].emplace_back();
				typeCount.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				typeCount.descriptorCount = shaderDescriptorSet.storageImages.size();
			}

			// descriptor set layout
			std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

			for (auto& [binding, uniformBuffer] : shaderDescriptorSet.uniformBuffers) {
				VkDescriptorSetLayoutBinding& layoutBinding = layoutBindings.emplace_back();
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				layoutBinding.descriptorCount = 1;
				layoutBinding.stageFlags = uniformBuffer->shaderStage;
				layoutBinding.pImmutableSamplers = nullptr;
				layoutBinding.binding = binding;

				VkWriteDescriptorSet& set = shaderDescriptorSet.writeDescriptorSets[uniformBuffer->name];
				set = {};
				set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				set.descriptorType = layoutBinding.descriptorType;
				set.descriptorCount = 1;
				set.dstBinding = layoutBinding.binding;
			}

			for (auto& [binding, imageSampler] : shaderDescriptorSet.imageSamplers) {
				auto& layoutBinding = layoutBindings.emplace_back();
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				layoutBinding.descriptorCount = 1;
				layoutBinding.stageFlags = imageSampler.shaderStage;
				layoutBinding.pImmutableSamplers = nullptr;
				layoutBinding.binding = binding;

				STLR_CORE_ASSERT(shaderDescriptorSet.uniformBuffers.find(binding) == shaderDescriptorSet.uniformBuffers.end(), "Binding is already present!");

				VkWriteDescriptorSet& set = shaderDescriptorSet.writeDescriptorSets[imageSampler.name];
				set = {};
				set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				set.descriptorType = layoutBinding.descriptorType;
				set.descriptorCount = 1;
				set.dstBinding = layoutBinding.binding;
			}

			for (auto& [bindingAndSet, imageSampler] : shaderDescriptorSet.storageImages) {
				auto& layoutBinding = layoutBindings.emplace_back();
				layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				layoutBinding.descriptorCount = 1;
				layoutBinding.stageFlags = imageSampler.shaderStage;
				layoutBinding.pImmutableSamplers = nullptr;

				uint32_t binding = bindingAndSet & 0xffffffff;
				uint32_t descriptorSet = ((unsigned long long)bindingAndSet >> 32);
				layoutBinding.binding = binding;

				STLR_CORE_ASSERT(shaderDescriptorSet.uniformBuffers.find(binding) == shaderDescriptorSet.uniformBuffers.end(), "Binding is already present!");
				STLR_CORE_ASSERT(shaderDescriptorSet.imageSamplers.find(binding) == shaderDescriptorSet.imageSamplers.end(), "Binding is already present!");

				VkWriteDescriptorSet& set = shaderDescriptorSet.writeDescriptorSets[imageSampler.name];
				set = {};
				set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				set.descriptorType = layoutBinding.descriptorType;
				set.descriptorCount = 1;
				set.dstBinding = layoutBinding.binding;
			}

			VkDescriptorSetLayoutCreateInfo descriptorLayout = {};
			descriptorLayout.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorLayout.pNext = nullptr;
			descriptorLayout.bindingCount = layoutBindings.size();
			descriptorLayout.pBindings = layoutBindings.data();

			STLR_CORE_INFO("Creating descriptor set {0} with {1} ubos, {2} samplers and {3} storage images", 
							set,
							shaderDescriptorSet.uniformBuffers.size(),
							shaderDescriptorSet.imageSamplers.size(),
							shaderDescriptorSet.storageImages.size());
			
			if (set >= m_DescriptorSetLayouts.size())
				m_DescriptorSetLayouts.resize(set + 1);
			VK_CHECK_RESULT(vkCreateDescriptorSetLayout(device, &descriptorLayout, nullptr, &m_DescriptorSetLayouts[set]));
		}
	}

	std::vector<VkDescriptorSetLayout> VulkanShader::getAllDescriptorSetLayouts() {
		std::vector<VkDescriptorSetLayout> result;
		result.reserve(m_DescriptorSetLayouts.size());
		for (auto& layout : m_DescriptorSetLayouts)
			result.emplace_back(layout);

		return result;
	}

	VulkanShader::ShaderMaterialDescriptorSet VulkanShader::allocateDescriptorSet(uint32_t set) {
		STLR_CORE_ASSERT(set < m_DescriptorSetLayouts.size());
		ShaderMaterialDescriptorSet result;

		if (m_ShaderDescriptorSets.empty())
			return result;
		
		result.Pool = nullptr;

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_DescriptorSetLayouts[set];
		VkDescriptorSet descriptorSet = VulkanRenderer::AllocateDesriptorSet(allocInfo);
		STLR_CORE_ASSERT(descriptorSet);
		result.DescriptorSets.push_back(descriptorSet);
		return result;
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
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		for (auto shader : m_ShaderModules) 
			vkDestroyShaderModule(VulkanDevice::GetInstance()->logicalDevice(),
				shader, nullptr);
		for (auto layout : m_DescriptorSetLayouts)
			vkDestroyDescriptorSetLayout(device, layout, nullptr);
	}

	const std::vector<VkPipelineShaderStageCreateInfo>& VulkanShader::getStageInfos() const {
		return m_StageInfos;
	}

	const VkWriteDescriptorSet* VulkanShader::getDescriptorSet(const std::string& name, uint32_t set) {
		STLR_CORE_ASSERT(set < m_ShaderDescriptorSets.size());
		STLR_CORE_ASSERT(m_ShaderDescriptorSets[set]);
		if (m_ShaderDescriptorSets.at(set).writeDescriptorSets.find(name) == m_ShaderDescriptorSets.at(set).writeDescriptorSets.end()) {
			STLR_CORE_WARN("Shader {0} does not contain requested descriptor set {1}", m_Name, name);
			return nullptr;
		}
		return &m_ShaderDescriptorSets.at(set).writeDescriptorSets.at(name);
	}

	const std::unordered_map<std::string, ShaderResourceDeclaration>& VulkanShader::getResources() const {
		return m_Resources;
	}
}