#pragma once

#include "Stellar/Core.h"

#include "RenderPass.h"

namespace Stellar {
    class STLR_API StandardRenderPass : public RenderPass {
    public:
        explicit StandardRenderPass(const VkFormat&);
        ~StandardRenderPass() override = default;
    };
}