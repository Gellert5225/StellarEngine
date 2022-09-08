#pragma once

#include "Stellar/Core.h"

#include "Stellar/Renderer/RendererAPI.h"

namespace Stellar {
    class STLR_API VulkanRenderer : public RendererAPI {
        void init() override;
        void shutDown() override;
        void beginRenderPass() override;
        void endRenderPass() override;
    };
}