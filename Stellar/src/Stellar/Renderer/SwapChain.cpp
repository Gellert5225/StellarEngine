#include "stlrpch.h"

#include "SwapChain.h"
#include "RendererAPI.h"

#include "Stellar/Platform/Vulkan/SwapChain/VulkanSwapChain.h"
#include "Stellar/Platform/Metal/SwapChain/MetalSwapChain.h"

#include "Stellar/Log.h"

namespace Stellar {

    SwapChain *SwapChain::Create() {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan:
                return new VulkanSwapChain();
            case RendererAPIType::Metal:
                return new MetalSwapChain();
            case RendererAPIType::None:
                break;
        }
        STLR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}