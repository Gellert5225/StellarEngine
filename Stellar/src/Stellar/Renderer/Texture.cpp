#include "Texture.h"

#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Texture/VulkanTexture.h"
#endif

#include "Stellar/Log.h"
#include "Stellar/Renderer/RendererAPI.h"

namespace Stellar {
    Texture2D* Texture2D::Create(const std::string& filePath) {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan:
            #if defined(__linux__) || defined(_WIN64)
                return new VulkanTexture(filePath);
            #endif
            case RendererAPIType::Metal:
            case RendererAPIType::None:
                break;
        }
        STLR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}