#pragma once

#include "Stellar/Core.h"
#include "RendererAPI.h"
#include "Stellar/Renderer/Buffer.h"

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Stellar {

    class STLR_API Renderer {
    public:
        static void Init();
        static void Shutdown();

        static void BeginRenderPass(VkCommandBuffer commandBuffer);
        static void EndRenderPass(VkCommandBuffer commandBuffer);

        static void SetClearColor(const glm::vec4& color);
        static void RenderGeometry(VkCommandBuffer commandBuffer,
                                   VertexBuffer* vertexBuffer,
                                   IndexBuffer* indexBuffer,
                                   uint32_t indexCount);

    };
}