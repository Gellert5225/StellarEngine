#include "stlrpch.h"

#include "MetalRenderer.h"
#include "Stellar/Application.h"
#include "Stellar/Platform/Metal/SwapChain/MetalSwapChain.h"
#include "Stellar/Platform/Metal/Device/MetalDevice.h"

namespace Stellar {
    void MetalRenderer::init() {

    }

    void MetalRenderer::shutDown() {
        //m_Encoder->release();
    }

    void MetalRenderer::beginScene(Stellar::Camera camera) {

    }

    void MetalRenderer::endScene() {
//        auto swapChain = (MetalSwapChain*)Application::Get().getWindow().getSwapChain();
//        swapChain->getCommandBuffer()->presentDrawable(swapChain->getCurrentFrameBuffer());
//        swapChain->getCommandBuffer()->commit();
    }

    void MetalRenderer::beginRenderPass() {
        auto swapChain = (MetalSwapChain*)Application::Get().getWindow().getSwapChain();
//        m_Encoder = swapChain->getCommandBuffer()->renderCommandEncoder(swapChain->getRenderPass());
        auto drawable = swapChain->getCurrentFrameBuffer();

        NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
        MTL::RenderPassDescriptor* mainPass = MTL::RenderPassDescriptor::alloc()->init();
        auto colorAttachment = mainPass->colorAttachments()->object(0);

        colorAttachment->setClearColor(MTL::ClearColor::Make(1, 0, 0, 1));
        colorAttachment->setLoadAction(MTL::LoadActionClear);
        colorAttachment->setStoreAction(MTL::StoreActionStore);
        colorAttachment->setTexture(drawable->texture());

        MTL::CommandBuffer* buffer = MetalDevice::GetInstance()->getCommandQueue()->commandBuffer();

        // This will segfault, as the renderCommandEncoderWithDescriptor: selector is 0x0
        // in the MTL::Private::Selector namespace. The selector was not loaded through
        // sel_registerName from objc/runtime.h.
        MTL::RenderCommandEncoder* encoder = buffer->renderCommandEncoder(mainPass);

        encoder->endEncoding();
        buffer->presentDrawable(drawable);
        buffer->commit();
        pPool->release();
    }

    void MetalRenderer::endRenderPass() {
        //m_Encoder->endEncoding();
    }

    void MetalRenderer::setClearColor(const glm::vec4 &color) {
        m_ClearColor = {color.r, color.g, color.b, color.a};
    }

    void MetalRenderer::renderGeometry(Stellar::Buffer *vertexBuffer,
                                       Stellar::Buffer *indexBuffer,
                                       uint32_t indexCount,
                                       const glm::mat4 &transform) {

    }
}

