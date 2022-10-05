#include "Texture.h"

#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Texture/VulkanTexture.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Texture/MetalTexture.h"
#endif

#include "Stellar/Core/Log.h"
#include "Stellar/Renderer/RendererAPI.h"

namespace Stellar {
    Texture2D* Texture2D::Create(const std::string& filePath) {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan:
            #if defined(__linux__) || defined(_WIN64)
                return new VulkanTexture(filePath);
            #endif
            case RendererAPIType::Metal:
            #if defined(__APPLE__)
                return new MetalTexture(filePath);
            #endif
            case RendererAPIType::None:
                break;
        }
        STLR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}