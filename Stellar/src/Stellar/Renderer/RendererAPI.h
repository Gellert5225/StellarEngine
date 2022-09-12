#pragma once

#include "Buffer.h"
#include "CommandBuffer.h"

#include <glm/glm.hpp>

namespace Stellar {
    enum class RendererAPIType {
        None = 0, Vulkan = 1, Metal = 2
    };

    class RendererAPI {
    public:
        virtual void init() = 0;
        virtual void shutDown() = 0;

        virtual void beginScene() = 0;

        virtual void beginRenderPass(CommandBuffer* commandBuffer) = 0;
        virtual void endRenderPass(CommandBuffer* commandBuffer) = 0;

        virtual void setClearColor(const glm::vec4& color) = 0;
        virtual void renderGeometry(CommandBuffer* commandBuffer,
                                    Buffer* vertexBuffers,
                                    Buffer* indexBuffer,
                                    uint32_t indexCount) = 0;

        static RendererAPIType Current() { return s_CurrentRendererAPI; }
    private:
        inline static RendererAPIType s_CurrentRendererAPI = RendererAPIType::Vulkan;

    };
}