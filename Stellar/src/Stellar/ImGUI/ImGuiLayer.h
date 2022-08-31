#pragma once

#include "Stellar/Layer.h"
#include "Stellar/Events/KeyEvent.h"
#include "Stellar/Events/MouseEvent.h"
#include "Stellar/Events/ApplicationEvent.h"

namespace Stellar {
    class STLR_API ImGuiLayer: public Layer {
    public:
        ImGuiLayer();
        ~ImGuiLayer() override;

        void begin() const;
        void end(VkCommandBuffer) const;

        void onAttach() override;
        void onDetach() override;
        void onUpdate() override;
        void onEvent(Event& e) override;
        void onImGuiRender() override;

    private:
        float m_Time = 0.0f;
    };
}