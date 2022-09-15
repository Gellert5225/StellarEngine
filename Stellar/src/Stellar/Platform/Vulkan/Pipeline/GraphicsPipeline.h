#pragma once

#include "Stellar/Core.h"

#include "VulkanPipeline.h"
#include "../Shader/Shader.h"

namespace Stellar {
    class STLR_API GraphicsPipeline : public VulkanPipeline {
    public:
        GraphicsPipeline(const std::string& vertShaderPath,
                         const std::string& fragShaderPath,
                         VkRenderPass renderPass);
    protected:
        void createDescriptorSetLayout() override;
        void createDescriptorPool() override;
    };
}