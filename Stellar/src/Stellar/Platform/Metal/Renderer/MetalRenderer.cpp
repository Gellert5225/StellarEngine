#include "stlrpch.h"

#include "MetalRenderer.h"
#include "Stellar/Core/Application.h"
#include "Stellar/Platform/Metal/SwapChain/MetalSwapChain.h"
#include "Stellar/Platform/Metal/Device/MetalDevice.h"
#include "Stellar/Platform/Metal/Shader/MetalShader.h"
#include "Stellar/Platform/Metal/Buffer/MetalBuffer.h"

namespace Stellar {
    void MetalRenderer::init() {
        auto shader = new MetalShader("../Resources/Shader/Metal/shader.metal");
        m_Pipeline = new MetalPipeline(shader);
        delete shader;
    }

    void MetalRenderer::shutDown() {
        delete m_Pipeline;
    }

    void MetalRenderer::beginScene(Stellar::Camera camera) {

    }

    void MetalRenderer::endScene() {
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
        m_Encoder->setRenderPipelineState(m_Pipeline->getPipelineState());
        m_Encoder->setVertexBuffer((MTL::Buffer*)vertexBuffer->getBuffer(), 0, 0);
        m_Encoder->setVertexBuffer((MTL::Buffer*)indexBuffer->getBuffer(), 0, 1);
        m_Encoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, 
                                  NS::UInteger(0), 
                                  NS::UInteger(3));
    }
}

