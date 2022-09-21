#include "stlrpch.h"
#include "Shader.h"

#include "Stellar/Renderer/RendererAPI.h"

#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Shader/VulkanShader.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Renderer/MetalRenderer.h"
#endif

#include "Stellar/Log.h"

namespace Stellar {
    Shader* Shader::Create(const std::string& filePath) {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan: 
            #if defined(__linux__) || defined(_WIN64)
                return new VulkanShader(filePath);
            #endif
        }
    }
}
