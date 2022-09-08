#pragma once

#include "Stellar/Core.h"
#include "Stellar/Renderer/RendererContext.h"
#include "Stellar/Platform/Vulkan/SwapChain/SwapChain.h"
#include "Stellar/Platform/Vulkan/Command/CommandBuffer.h"
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

    private:

    };
}