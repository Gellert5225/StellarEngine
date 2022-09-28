#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/Window.h"

#include "Stellar/Renderer/RendererContext.h"
#include "Stellar/Platform/Vulkan/SwapChain/VulkanSwapChain.h"
#include "Stellar/Platform/Vulkan/Command/VulkanCommandBuffer.h"
#include "Stellar/Platform/Vulkan/Pipeline/GraphicsPipeline.h"
#include "Stellar/Platform/Vulkan/Instance/VulkanInstance.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

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