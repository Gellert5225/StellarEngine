#include "stlrpch.h"

#include "RendererContext.h"
#include "Stellar/Platform/Vulkan/Renderer/VulkanRendererContext.h"
#include "Stellar/Log.h"

namespace Stellar {
    RendererContext *RendererContext::Create() {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan:
                return new VulkanRendererContext();
            case RendererAPIType::Metal:
            STLR_CORE_ASSERT(false, "Metal is not yet supported");
                break;
            case RendererAPIType::None:
                break;
        }
        STLR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}