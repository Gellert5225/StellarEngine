#pragma once

#include "Buffer.h"
#include "CommandBuffer.h"
#include "Camera.h"
#include "Uniforms.h"

#include <glm/glm.hpp>

namespace Stellar {
    enum class RendererAPIType {
        None = 0, Vulkan = 1, Metal = 2
    };

    class RendererAPI {
    public:
        virtual void init() = 0;
        virtual void shutDown() = 0;

        virtual void beginRenderPass() = 0;
        virtual void endRenderPass() = 0;

        virtual void setClearColor(const glm::vec4& color) = 0;
        virtual void renderGeometry(Buffer* vertexBuffers,
                                    Buffer* indexBuffer,
                                    const glm::vec3& color,
                                    uint32_t indexCount = 0,
                                    const glm::mat4& transform = {}) = 0;
        
        virtual void bindUbo(const GlobalUniforms& ubo) = 0;

        static RendererAPIType Current() { return s_CurrentRendererAPI; }
    protected:
        CommandBuffer* m_CommandBuffer{};
        Camera m_Camera;
    private:
#ifdef __APPLE__
#define STLR_RENDERER_API RendererAPIType::Metal
#else
#define STLR_RENDERER_API RendererAPIType::Vulkan
#endif
        inline static RendererAPIType s_CurrentRendererAPI = STLR_RENDERER_API;

    };
}