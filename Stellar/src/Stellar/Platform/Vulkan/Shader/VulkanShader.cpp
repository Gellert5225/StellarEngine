#include "stlrpch.h"
#include "VulkanShader.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Log.h"

#include "Stellar/Platform/Vulkan/Shader/VulkanShaderCompiler.h"

namespace Stellar {

    VulkanShader::VulkanShader(const std::string& filePath) {
        auto source = ReadFile(filePath);
        STLR_CORE_INFO("Preproecssing Shader: {0}", filePath.substr(filePath.find_last_of("/") + 1));
        auto result = VulkanShaderCompiler::PreProcess(source);
        std::unordered_map<ShaderType, std::vector<uint32_t>> spv;

        STLR_CORE_INFO("Compiling Shader: {0}", filePath.substr(filePath.find_last_of("/") + 1));
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
        if (vkCreateShaderModule(device,
                                 &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            throw std::runtime_error("Failed to create shader module!");

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