#pragma once

#include "Buffer.h"
#include "CommandBuffer.h"
#include "Camera.h"

#include <glm/glm.hpp>

namespace Stellar {
    enum class RendererAPIType {
        None = 0, Vulkan = 1, Metal = 2
    };

    class RendererAPI {
    public:
        virtual void init() = 0;
        virtual void shutDown() = 0;

        virtual void beginScene(Camera camera) = 0;
        virtual void endScene() = 0;

        virtual void beginRenderPass() = 0;
        virtual void endRenderPass() = 0;

        virtual void setClearColor(const glm::vec4& color) = 0;
        virtual void renderGeometry(Buffer* vertexBuffers,
                                    Buffer* indexBuffer,
                                    uint32_t indexCount,
                                    const glm::mat4& transform) = 0;

        static RendererAPIType Current() { return s_CurrentRendererAPI; }
    protected:
        CommandBuffer* m_CommandBuffer{};
        Camera m_Camera;
    private:
        inline static RendererAPIType s_CurrentRendererAPI = RendererAPIType::Vulkan;

    };
}