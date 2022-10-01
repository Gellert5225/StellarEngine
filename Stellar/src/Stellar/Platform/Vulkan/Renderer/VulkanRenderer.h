#pragma once

#include "Stellar/Core/Core.h"

#include "Stellar/Renderer/RendererAPI.h"
#include "Stellar/Renderer/Buffer.h"
#include "Stellar/Platform/Vulkan/SwapChain/VulkanSwapChain.h"
#include "Stellar/Core/Application.h"

#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API VulkanRenderer : public RendererAPI {
    public:
        void init() override;
        void shutDown() override;

        void beginRenderPass() override;
        void endRenderPass() override;

        void setClearColor(const glm::vec4& color) override;
        void renderGeometry(Buffer* vertexBuffer,
                            Buffer* indexBuffer,
                            Texture2D* texture,
                            const glm::vec3& color,
                            uint32_t indexCount,
                            const glm::mat4& transform) override;
        
        void bindUbo(const GlobalUniforms& ubo) override;

        // vulkan
        static GraphicsPipeline* GetPipeline();
    private:
        GraphicsPipeline* m_GraphicsPipeline = nullptr;
        VkClearColorValue m_ClearColor = {{0.66f, 0.9f, 0.96f, 1.0f}};

        Buffer* m_UniformBuffer{};

        VkDescriptorSet m_UboDescriptorSet;

        void createUboDescriptorSet();
    };
}