#include "stlrpch.h"
#include "Shader.h"

#include "Stellar/Render/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Log.h"

namespace Stellar {
    Shader* Shader::s_Instance = nullptr;

    Shader* Shader::GetInstance() {
        if (s_Instance == nullptr)
            s_Instance = new Shader();
        return s_Instance;
    }

    std::vector<char> Shader::ReadFile(const std::string& fileName) {
        std::ifstream file(fileName, std::ios::ate | std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("Failed to open file");

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }

    VkShaderModule Shader::CreateShaderModule(const std::vector<char>& code) {
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

    VkShaderModule Shader::getShaderModule(const std::string& filePath) {
        if (loadedShaders.find(filePath) == loadedShaders.end()) {
            loadedShaders[filePath] = CreateShaderModule(ReadFile(filePath));
            STLR_CORE_INFO("Shader {0} loaded", filePath);
        }
        return loadedShaders[filePath];
    }

    Shader::~Shader() {
        cleanUp();
    }

    void Shader::cleanUp() {

    }
}