#pragma once

#include "Stellar/Core.h"
#include "Stellar/Renderer/RendererContext.h"
#include "Stellar/Platform/Vulkan/SwapChain/SwapChain.h"
#include "Stellar/Platform/Vulkan/Command/VulkanCommandBuffer.h"
#include "Stellar/Platform/Vulkan/Pipeline/GraphicsPipeline.h"
#include "Stellar/Platform/Vulkan/Instance/VulkanInstance.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

#include "Stellar/Window.h"

#include <memory>

namespace Stellar {
    class STLR_API VulkanRendererContext : public RendererContext {
    public:
        VulkanRendererContext();
        ~VulkanRendererContext() override;

        [[nodiscard]] std::string getGPUInfo() const override;
        [[nodiscard]] std::string getGraphicsAPI() const override;

    private:

    };
}