#include "stlrpch.h"
#include "Shader.h"

#include "Stellar/Renderer/RendererAPI.h"

#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Shader/VulkanShader.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Shader/MetalShader.h"
#endif

#include "Stellar/Log.h"

namespace Stellar {
    Shader* Shader::Create(const std::string& filePath) {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan:
            #if defined(__linux__) || defined(_WIN64)
                return new VulkanShader(filePath);
            #endif
            case RendererAPIType::Metal:
            #if defined(__APPLE__)
                return new MetalShader(filePath);
            #endif
            case RendererAPIType::None:
                break;
        }
        STLR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

    std::vector<char> Shader::ReadFile(const std::string& fileName) {
        std::ifstream file(fileName, std::ios::ate | std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error("Failed to open shader file " + fileName);

        size_t fileSize = (size_t) file.tellg();
        std::vector<char> buffer(fileSize);
        file.seekg(0);
        file.read(buffer.data(), fileSize);
        file.close();

        return buffer;
    }
}
