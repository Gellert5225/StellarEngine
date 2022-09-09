#pragma once

#include "Stellar/Layer.h"
#include "Stellar/Events/KeyEvent.h"
#include "Stellar/Events/MouseEvent.h"
#include "Stellar/Events/ApplicationEvent.h"

#include "Stellar/Renderer/CommandBuffer.h"

#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API ImGuiLayer: public Layer {
    public:
        ImGuiLayer();
        ~ImGuiLayer() override;

        void begin() const;
        void end(CommandBuffer* commandBuffer) const;

        void onAttach() override;
        void onDetach() override;
        void onImGuiRender() override;

    private:
        VkDescriptorPool m_DescriptorPool;
        float m_Time = 0.0f;
    };
}