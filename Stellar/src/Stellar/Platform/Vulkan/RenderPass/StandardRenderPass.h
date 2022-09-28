#pragma once

#include "Stellar/Core/Core.h"

#include "RenderPass.h"

namespace Stellar {
    class STLR_API StandardRenderPass : public RenderPass {
    public:
        explicit StandardRenderPass(const VkFormat& swapChainImageFormat);
        ~StandardRenderPass() override = default;
    };
}