#pragma once

#include "RendererAPI.h"
#include "Buffer.h"
#include "CommandBuffer.h"

#include "Stellar/Core.h"

#include <glm/glm.hpp>

namespace Stellar {

    class STLR_API Renderer {
    public:
        static void Init();
        static void Shutdown();

        static void BeginRenderPass(CommandBuffer* commandBuffer);
        static void EndRenderPass(CommandBuffer* commandBuffer);

        static void SetClearColor(const glm::vec4& color);
        static void RenderGeometry(CommandBuffer* commandBuffer,
                                   Buffer* vertexBuffer,
                                   Buffer* indexBuffer,
                                   uint32_t indexCount);

        static uint32_t GetCurrentFrameIndex();

    };
}