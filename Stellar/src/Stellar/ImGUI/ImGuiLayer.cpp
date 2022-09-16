#include "stlrpch.h"
#include "ImGuiLayer.h" 

#include "Stellar/Renderer/RendererAPI.h"

#include "Stellar/Log.h"

#if defined __linux__ || defined _WIN64_
#include "Stellar/Platform/Vulkan/ImGUI/VulkanImGuiLayer.h"
#endif

#if defined __APPLE__
#include "Stellar/Platform/Metal/ImGui/MetalImGuiLayer.h"
#endif

namespace Stellar {
    ImGuiLayer *ImGuiLayer::Create() {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan: 
                #if defined __linux__ || defined _WIN64_
                    return new VulkanImGuiLayer();
                #endif
            case RendererAPIType::Metal:  
                #if defined __APPLE__
                    return new MetalImGuiLayer();
                #endif
            case RendererAPIType::None:   return nullptr;
        }
        STLR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;

    }
}