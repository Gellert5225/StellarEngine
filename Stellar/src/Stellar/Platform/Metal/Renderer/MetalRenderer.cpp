#include "stlrpch.h"

#include "MetalRenderer.h"
#include "Stellar/Application.h"
#include "Stellar/Platform/Metal/SwapChain/MetalSwapChain.h"
#include "Stellar/Platform/Metal/Device/MetalDevice.h"

namespace Stellar {
    void MetalRenderer::init() {

    }

    void MetalRenderer::shutDown() {

    }

    void MetalRenderer::beginScene(Stellar::Camera camera) {

    }

    void MetalRenderer::endScene() {
        auto swapChain = (MetalSwapChain*)Application::Get().getWindow().getSwapChain();
        //m_CommandBuffer->presentDrawable(swapChain->getCurrentFrameBuffer());
        m_CommandBuffer->commit();
        m_CommandBuffer->release();
    }

    void MetalRenderer::beginRenderPass() {
        auto swapChain = (MetalSwapChain*)Application::Get().getWindow().getSwapChain();

        auto colorAttachment = swapChain->getRenderPass()->colorAttachments()->object(0);
        colorAttachment->setClearColor(m_ClearColor);
        colorAttachment->setLoadAction(MTL::LoadActionClear);
        colorAttachment->setStoreAction(MTL::StoreActionStore);
        colorAttachment->setTexture(swapChain->getCurrentFrameBuffer()->texture());

        m_CommandBuffer = MetalDevice::GetInstance()->getCommandQueue()->commandBuffer();

        m_Encoder = m_CommandBuffer->renderCommandEncoder(swapChain->getRenderPass());
    }

    void MetalRenderer::endRenderPass() {
        m_Encoder->endEncoding();
        m_Encoder->release();
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

