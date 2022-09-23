#pragma once

#include "Stellar/Layer.h"

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