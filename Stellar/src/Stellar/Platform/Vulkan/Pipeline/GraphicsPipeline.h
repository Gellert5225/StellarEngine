#pragma once

#include "Stellar/Core.h"

#include "VulkanPipeline.h"
#include "Stellar/Platform/Vulkan/Shader/VulkanShader.h"

namespace Stellar {
    class STLR_API GraphicsPipeline : public VulkanPipeline {
    public:
        GraphicsPipeline(const std::string& shaderPath);
    protected:
        void createDescriptorSetLayout() override;
        void createDescriptorPool() override;
    };
}