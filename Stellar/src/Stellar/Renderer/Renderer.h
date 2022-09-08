#pragma once

#include "Stellar/Core.h"
#include "RendererAPI.h"

#include <vulkan/vulkan.h>

namespace Stellar {

    class STLR_API Renderer {
    public:
        static void Init();
        static void Shutdown();

        static void BeginRenderPass(VkCommandBuffer commandBuffer);
        static void EndRenderPass(VkCommandBuffer commandBuffer);

        static void RenderGeometry();

    };
}