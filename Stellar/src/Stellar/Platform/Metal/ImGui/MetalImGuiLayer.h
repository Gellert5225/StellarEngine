#pragma once

#include "Stellar/Core.h"
#include "Stellar/ImGUI/ImGuiLayer.h"
#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {

    class STLR_API MetalImGuiLayer : public ImGuiLayer {
    public:
        MetalImGuiLayer();
        ~MetalImGuiLayer() override;

        void begin() override;
        void end() const override;

        void onAttach() override;
        void onDetach() override;
        void onImGuiRender() override;

    private:
        MTL::CommandBuffer* m_CommandBuffer = nullptr;
        MTL::RenderCommandEncoder* m_Encoder;
    };

}