#include "stlrpch.h"
#include "ImGuiLayer.h" 

#include "Stellar/Renderer/RendererAPI.h"

#include "Stellar/Log.h"

#include "Stellar/Platform/Vulkan/ImGUI/VulkanImGuiLayer.h"
#include "Stellar/Platform/Metal/ImGui/MetalImGuiLayer.h"

namespace Stellar {
    ImGuiLayer *ImGuiLayer::Create() {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan: return new VulkanImGuiLayer();
            case RendererAPIType::Metal:  return new MetalImGuiLayer();
            case RendererAPIType::None:   return nullptr;
        }
        STLR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;

    }
}