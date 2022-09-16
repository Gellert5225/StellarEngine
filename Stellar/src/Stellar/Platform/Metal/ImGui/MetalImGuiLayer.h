#pragma once

#include "Stellar/Core.h"
#include "Stellar/ImGUI/ImGuiLayer.h"

namespace Stellar {

    class STLR_API MetalImGuiLayer : public ImGuiLayer {
    public:
        MetalImGuiLayer();
        ~MetalImGuiLayer() override;

        void begin() const override;
        void end() const override;

        void onAttach() override;
        void onDetach() override;
        void onImGuiRender() override;

    private:
    };

}