#include "stlrpch.h"

#include "Renderer.h"

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Renderer/MetalRenderer.h"
#endif

#if defined __GNUC__ || defined _WIN64_
#include "Stellar/Platform/Vulkan/Renderer/VulkanRenderer.h"
#endif

#include "Stellar/Log.h"

namespace Stellar {
    static RendererAPI* s_RendererAPI = nullptr;

    static RendererAPI* InitRendererAPI() {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan: 
                #if defined __GNUC__ || defined _WIN64_
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
        s_RendererAPI = InitRendererAPI();
        s_RendererAPI->init();
    }

    void Renderer::Shutdown() {
        s_RendererAPI->shutDown();
    }

    void Renderer::BeginRenderPass() {
        s_RendererAPI->beginRenderPass();
    }

    void Renderer::EndRenderPass() {
        s_RendererAPI->endRenderPass();
    }

    void Renderer::RenderGeometry(Buffer* vertexBuffer,
                                  Buffer* indexBuffer,
                                  uint32_t indexCount,
                                  const glm::mat4& transform) {
        s_RendererAPI->renderGeometry(vertexBuffer, indexBuffer, indexCount, transform);
    }

    void Renderer::SetClearColor(const glm::vec4 &color) {
        s_RendererAPI->setClearColor(color);
    }

    uint32_t Renderer::GetCurrentFrameIndex() {
        return Application::Get().getWindow().getSwapChain()->getCurrentFrameIndex();
    }

    void Renderer::BeginScene(Camera camera) {
        s_RendererAPI->beginScene(camera);
    }

    void Renderer::EndScene() {
        s_RendererAPI->endScene();
    }
}