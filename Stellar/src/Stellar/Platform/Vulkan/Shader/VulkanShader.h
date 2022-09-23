#pragma once

#include "Stellar/Core.h"
#include "Stellar/Renderer/Shader.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Stellar {
    class STLR_API VulkanShader : public Shader {
    public:
        explicit VulkanShader(const std::string& filePath);
        ~VulkanShader();

        VkShaderModule getShaderModule(const std::string& filePath);
        VkPipelineShaderStageCreateInfo getStageInfo() const;
    private:
        VkPipelineShaderStageCreateInfo m_StageInfo{};
        
        const std::string extractType(const std::string& filePath) const;
        static VkShaderModule CreateShaderModule(const std::vector<char>& code);

        VkShaderModule m_ShaderModule;

        std::unordered_map<std::string, VkShaderModule> loadedShaders;
    };
}