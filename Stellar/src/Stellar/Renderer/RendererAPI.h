#pragma once

#include "Stellar/Renderer/Buffer.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Stellar {
    enum class RendererAPIType {
        None = 0, Vulkan = 1, Metal = 2
    };

    class RendererAPI {
    public:
        virtual void init() = 0;
        virtual void shutDown() = 0;

        virtual void beginRenderPass(VkCommandBuffer commandBuffer) = 0;
        virtual void endRenderPass(VkCommandBuffer commandBuffer) = 0;

        virtual void setClearColor(const glm::vec4& color) = 0;
        virtual void renderGeometry(VkCommandBuffer commandBuffer,
                                    VertexBuffer* vertexBuffers,
                                    IndexBuffer* indexBuffer,
                                    uint32_t indexCount) = 0;

        static RendererAPIType Current() { return s_CurrentRendererAPI; }
    private:
        inline static RendererAPIType s_CurrentRendererAPI = RendererAPIType::Vulkan;

    };
}