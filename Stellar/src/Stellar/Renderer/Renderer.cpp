#include "stlrpch.h"

#include "Renderer.h"

#include "Stellar/Platform/Vulkan/Renderer/VulkanRenderer.h"
#include "Stellar/Log.h"

namespace Stellar {
    static RendererAPI* s_RendererAPI = nullptr;

    static RendererAPI* InitRendererAPI() {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan: return new VulkanRenderer();
            case RendererAPIType::Metal:
                STLR_CORE_ASSERT(false, "Metal is not yet supported");
                break;
            case RendererAPIType::None:
                break;
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

    void Renderer::BeginRenderPass(CommandBuffer* commandBuffer) {
        s_RendererAPI->beginRenderPass(commandBuffer);
    }

    void Renderer::EndRenderPass(CommandBuffer* commandBuffer) {
        s_RendererAPI->endRenderPass(commandBuffer);
    }

    void Renderer::RenderGeometry(CommandBuffer* commandBuffer,
                                  Buffer* vertexBuffer,
                                  Buffer* indexBuffer,
                                  uint32_t indexCount) {
        s_RendererAPI->renderGeometry(commandBuffer, vertexBuffer, indexBuffer, indexCount);
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
}