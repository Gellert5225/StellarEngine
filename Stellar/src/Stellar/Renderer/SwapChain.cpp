#include "stlrpch.h"

#include "SwapChain.h"
#include "RendererAPI.h"

#if defined __GNUC__ || defined _WIN64_
#include "Stellar/Platform/Vulkan/SwapChain/VulkanSwapChain.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/SwapChain/MetalSwapChain.h"
#endif

#include "Stellar/Log.h"

namespace Stellar {

    SwapChain *SwapChain::Create() {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan:
                #if defined __GNUC__ || defined _WIN64_
                    return new VulkanSwapChain();
                #endif
            case RendererAPIType::Metal:
                #if defined __APPLE__
                    return new MetalSwapChain();
                #endif
            case RendererAPIType::None:
                break;
        }
        STLR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}