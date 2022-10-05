#pragma once

#include "Stellar/Core/Layer.h"

#include "Stellar/Renderer/FrameBuffer.h"
#include <imgui.h>

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