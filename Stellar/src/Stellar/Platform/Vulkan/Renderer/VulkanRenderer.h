#pragma once

#include "Stellar/Core.h"

#include "Stellar/Renderer/RendererAPI.h"
#include "Stellar/Platform/Vulkan/SwapChain/SwapChain.h"
#include "Stellar/Application.h"

#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API VulkanRenderer : public RendererAPI {
    public:
        void init() override;
        void shutDown() override;

        void beginRenderPass(VkCommandBuffer commandBuffer) override;
        void endRenderPass(VkCommandBuffer commandBuffer) override;

        void setClearColor(const glm::vec4& color) override;
        void renderGeometry() override;

    private:
        GraphicsPipeline* m_GraphicsPipeline = nullptr;
        VkClearColorValue m_ClearColor = {{0.66f, 0.9f, 0.96f, 1.0f}};
    };
}