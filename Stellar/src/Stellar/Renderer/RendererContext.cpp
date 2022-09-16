#include "stlrpch.h"

#include "RendererContext.h"
#include "Stellar/Platform/Vulkan/Renderer/VulkanRendererContext.h"
#include "Stellar/Platform/Metal/Renderer/MetalRendererContext.h"
#include "Stellar/Log.h"

namespace Stellar {
    RendererContext* RendererContext::Create() {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan:
                #if defined __linux__ || defined _WIN64_
                    return new VulkanRendererContext();
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