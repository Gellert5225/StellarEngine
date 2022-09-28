#pragma once

#include "RendererAPI.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#include "Camera.h"
#include "Stellar/Core.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include "Uniforms.h"

namespace Stellar {

    class STLR_API Renderer {
    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(Camera camera);
        static void EndScene();

        static void BeginRenderPass();
        static void EndRenderPass();

        static void SetClearColor(const glm::vec4& color);
        static void RenderGeometry(Buffer* vertexBuffer,
                                   Buffer* indexBuffer,
                                   const glm::vec3& color,
                                   uint32_t indexCount = 0,
                                   const glm::mat4& transform = {});

        static ShaderLibrary* GetShaderLibrary();
        
        static uint32_t GetCurrentFrameIndex();
    };
}