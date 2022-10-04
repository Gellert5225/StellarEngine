#pragma once

#include "Stellar/Core/Layer.h"

#if defined __linux__ || defined _WIN64
#include "Stellar/Platform/Vulkan/ImGui/imgui_impl_vulkan.h"
#include "Stellar/Platform/Vulkan/Image/VulkanImage.h"
#include "Stellar/Platform/Vulkan/Texture/VulkanTexture.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanFrameBuffer.h"
#endif

#include "Stellar/Renderer/FrameBuffer.h"

namespace Stellar {
    class STLR_API ImGuiLayer: public Layer {
    public:
        virtual void begin() = 0;
        virtual void end() const = 0;

        static ImGuiLayer* Create();
    protected:
        float m_Time = 0.0f;

    };
}

namespace Stellar::UI {
    void Image(Image2D* image, const ImVec2& size);
    void ImageFromFB(FrameBuffer* frameBuffer, const ImVec2& size);
}