#include "stlrpch.h"

#include "RendererContext.h"
#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Renderer/VulkanRendererContext.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Renderer/MetalRendererContext.h"
#endif
#include "Stellar/Core/Log.h"

namespace Stellar {
    Ref<RendererContext> RendererContext::Create() {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan:
                #if defined __linux__ || defined _WIN64
                    return CreateRef<VulkanRendererContext>();
                #endif
            case RendererAPIType::Metal:
                #if defined __APPLE__
                    return new MetalRendererContext();
                #endif
            case RendererAPIType::None:
                break;
        }
        STLR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

}