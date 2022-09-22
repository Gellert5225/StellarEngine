#include "stlrpch.h"
#include "VulkanShader.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Log.h"

namespace Stellar {

    VulkanShader::VulkanShader(const std::string& filePath) {
        m_ShaderModule = CreateShaderModule(ReadFile(filePath));
        const std::string shaderType = extractType(filePath);

        VkPipelineShaderStageCreateInfo shaderStageInfo{};
        shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.stage = shaderType == "vert" ? VK_SHADER_STAGE_VERTEX_BIT : VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStageInfo.module = m_ShaderModule;
        shaderStageInfo.pName = "main";

        m_StageInfo = shaderStageInfo;
    }

    const std::string VulkanShader::extractType(const std::string& filePath) const {
        auto fileName = filePath.substr(filePath.find_last_of("/") + 1);
        STLR_CORE_INFO("Shader file name: {0}", fileName);
        return fileName.substr(fileName.find_first_of(".") + 1, 4);
    }

    VkShaderModule VulkanShader::CreateShaderModule(const std::vector<char>& code) {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(VulkanDevice::GetInstance()->logicalDevice(),
                                 &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            throw std::runtime_error("Failed to create shader module!");

        return shaderModule;
    }

    VkShaderModule VulkanShader::getShaderModule(const std::string& filePath) {
        if (loadedShaders.find(filePath) == loadedShaders.end()) {
            loadedShaders[filePath] = CreateShaderModule(ReadFile(filePath));
            STLR_CORE_INFO("Shader {0} loaded", filePath);
        }
        return loadedShaders[filePath];
    }

    VulkanShader::~VulkanShader() {
        vkDestroyShaderModule(VulkanDevice::GetInstance()->logicalDevice(),
            m_ShaderModule, nullptr);
    }

    VkPipelineShaderStageCreateInfo VulkanShader::getStageInfo() const {
        return m_StageInfo;
    }
}