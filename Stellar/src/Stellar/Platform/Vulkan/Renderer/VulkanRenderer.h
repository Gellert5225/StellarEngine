#pragma once

#include "Stellar/Core.h"

#include "Stellar/Renderer/RendererAPI.h"
#include "Stellar/Renderer/Buffer.h"
#include "Stellar/Platform/Vulkan/SwapChain/SwapChain.h"
#include "Stellar/Application.h"

#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API VulkanRenderer : public RendererAPI {
    public:
        void init() override;
        void shutDown() override;

        void beginScene() override;

        void beginRenderPass(CommandBuffer* commandBuffer) override;
        void endRenderPass(CommandBuffer* commandBuffer) override;

        void setClearColor(const glm::vec4& color) override;
        void renderGeometry(CommandBuffer* commandBuffer,
                            Buffer* vertexBuffer,
                            Buffer* indexBuffer,
                            uint32_t indexCount) override;

    private:
        GraphicsPipeline* m_GraphicsPipeline = nullptr;
        VkClearColorValue m_ClearColor = {{0.66f, 0.9f, 0.96f, 1.0f}};

        Buffer* m_UniformBuffer{};
    };
}