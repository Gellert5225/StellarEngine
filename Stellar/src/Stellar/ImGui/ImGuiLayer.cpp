#include "stlrpch.h"
#include "ImGuiLayer.h" 

#include "Stellar/Renderer/RendererAPI.h"
#include "Stellar/Renderer/Image.h"

#include "Stellar/Core/Log.h"

#if defined __linux__ || defined _WIN64
#include "Stellar/Platform/Vulkan/ImGui/imgui_impl_vulkan.h"
#include "Stellar/Platform/Vulkan/ImGui/VulkanImGuiLayer.h"
#include "Stellar/Platform/Vulkan/Image/VulkanImage.h"
#include "Stellar/Platform/Vulkan/Texture/VulkanTexture.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanFrameBuffer.h"
#elif defined __APPLE__
#include "Stellar/Platform/Metal/ImGui/imgui_impl_metal.h"
#include "Stellar/Platform/Metal/ImGui/MetalImGuiLayer.h"
#include "Stellar/Platform/Metal/Buffer/MetalFrameBuffer.h"
#endif

namespace Stellar {
    ImGuiLayer *ImGuiLayer::Create() {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan: 
                #if defined __linux__ || defined _WIN64
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

namespace Stellar::UI {
    void STLR_API Image(Image2D* image, const ImVec2& size) {
        #if defined __linux__ || defined _WIN64
        auto imageInfo = (VulkanImageInfo*)image->getImageInfo();
        const auto textureID = ImGui_ImplVulkan_AddTexture(imageInfo->sampler, imageInfo->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        ImGui::Image(textureID, size);
        #endif
    }

    void STLR_API ImageFromFB(FrameBuffer* frameBuffer, const ImVec2& size) {
        #if defined __linux__ || defined _WIN64
        auto imageInfo = (VulkanImageInfo*)frameBuffer->getAttachmentImage()->getImageInfo();
        const auto textureID = ImGui_ImplVulkan_AddTexture(imageInfo->sampler, imageInfo->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        ImGui::Image(textureID, size);
        #elif defined __APPLE__
        ImGui::Image(((MetalFrameBuffer*)frameBuffer)->getAttachmentTexture(), size);
        #endif
    }
}