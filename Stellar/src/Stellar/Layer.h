#pragma once

#include "Core.h"
#include "Events/Event.h"
#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API Layer {
    public: 
        explicit Layer(const std::string& name = "Layer");
        virtual ~Layer();

        virtual void onAttach() {}
        virtual void onDetach() {}
        virtual void onUpdate() {}
        virtual void onEvent(Event& e) {}
        virtual void onImGuiRender() {}

        [[nodiscard]] inline const std::string& getName() const { return m_DebugName; }
    protected:
        std::string m_DebugName;
    };
}