#pragma once

#include "RendererAPI.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#include "Camera.h"
#include "Shader.h"
#include "Uniforms.h"
#include "Texture.h"

#include "Stellar/Core/Core.h"

#include <glm/glm.hpp>

namespace Stellar {

    class STLR_API Renderer {
    public:
        static void Init();
        static void Shutdown();

        static void BeginRenderPass();
        static void EndRenderPass();

        static void SetClearColor(const glm::vec4& color);
        static void RenderGeometry(Buffer* vertexBuffer,
                                   Buffer* indexBuffer,
                                   Texture2D* texture,
                                   const glm::vec3& color,
                                   uint32_t indexCount = 0,
                                   const glm::mat4& transform = {});

        static void BindUbo(const GlobalUniforms& ubo);

        static ShaderLibrary* GetShaderLibrary();
        
        static uint32_t GetCurrentFrameIndex();
    };
}