#include "stlrpch.h"
#include "Renderer.h"

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Renderer/MetalRenderer.h"
#endif

#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Renderer/VulkanRenderer.h"
#endif

#include "Stellar/Core/Log.h"
#include "Stellar/Core/Application.h"
#include "Stellar/Renderer/Renderer2D.h"

namespace Stellar {
    struct RendererData {
        ShaderLibrary* m_ShaderLibrary;
    };

    static RendererAPI* s_RendererAPI = nullptr;
    static RendererData* s_Data = nullptr;

    static RendererAPI* InitRendererAPI() {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan: 
                #if defined __linux__ || defined _WIN64
                    return new VulkanRenderer();
                #endif
            case RendererAPIType::Metal: 
                #if defined __APPLE__
                    return new MetalRenderer();
                #endif
            case RendererAPIType::None: break;
        }
        STLR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

    void Renderer::Init() {
        s_Data = new RendererData();
        s_Data->m_ShaderLibrary = new ShaderLibrary();
        
        s_RendererAPI = InitRendererAPI();

        Renderer::GetShaderLibrary()->load("shader");
        Renderer::GetShaderLibrary()->load("grid");
        Renderer::GetShaderLibrary()->load("basicShader");

        s_RendererAPI->init();
        Renderer2D::Init();
    }

    void Renderer::Shutdown() {
        delete Renderer::GetShaderLibrary();
        s_RendererAPI->shutDown();
        Renderer2D::ShutDown();
    }

    void Renderer::BeginRenderPass() {
        s_RendererAPI->beginRenderPass();
    }

    void Renderer::EndRenderPass() {
        s_RendererAPI->endRenderPass();
    }

    void Renderer::RenderGeometry(Buffer* vertexBuffer,
                                  Buffer* indexBuffer,
                                  Texture2D* texture,
                                  const glm::vec4& color,
                                  uint32_t indexCount,
                                  const glm::mat4& transform) {
        s_RendererAPI->renderGeometry(vertexBuffer, indexBuffer, texture, color, indexCount, transform);
    }

    void Renderer::RenderGrid(Buffer* vertexBuffer, Buffer* indexBuffer, uint32_t indexCount) {
        s_RendererAPI->renderGrid(vertexBuffer, indexBuffer, indexCount);
    }

    void Renderer::SetClearColor(const glm::vec4 &color) {
        s_RendererAPI->setClearColor(color);
    }

    uint32_t Renderer::GetCurrentFrameIndex() {
        return Application::Get().getWindow().getSwapChain()->getCurrentFrameIndex();
    }

    void Renderer::BindUbo(const GlobalUniforms& ubo) {
        s_RendererAPI->bindUbo(ubo);
    }

    ShaderLibrary* Renderer::GetShaderLibrary() {
        return s_Data->m_ShaderLibrary;
    }

    FrameBuffer* Renderer::GetFrameBuffer() {
        return s_RendererAPI->getFrameBuffer();
    }

    void Renderer::ResizeFrameBuffer(uint32_t width, uint32_t height) {
        s_RendererAPI->resizeFrameBuffer(width, height);
    }
}