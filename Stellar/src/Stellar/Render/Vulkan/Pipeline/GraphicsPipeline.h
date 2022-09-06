#pragma once

#include "Stellar/Core.h"

#include "Pipeline.h"
#include "../Shader/Shader.h"

namespace Stellar {
    class STLR_API GraphicsPipeline : public Pipeline {
    public:
        GraphicsPipeline(const std::string& vertShaderPath,
                         const std::string& fragShaderPath,
                         const VkExtent2D& swapChainExtent,
                         VkRenderPass renderPass);
    };
}