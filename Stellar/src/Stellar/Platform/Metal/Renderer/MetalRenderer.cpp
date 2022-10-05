#include "stlrpch.h"

#include "MetalRenderer.h"
#include "Stellar/Core/Application.h"
#include "Stellar/Platform/Metal/SwapChain/MetalSwapChain.h"
#include "Stellar/Platform/Metal/Device/MetalDevice.h"
#include "Stellar/Platform/Metal/Shader/MetalShader.h"
#include "Stellar/Platform/Metal/Buffer/MetalBuffer.h"

namespace Stellar {
    void MetalRenderer::init() {
        // create framebuffer
        FrameBufferSpec framebufferSpec;
        framebufferSpec.width = 1280;
        framebufferSpec.height = 720;
        m_FrameBuffer = FrameBuffer::Create(framebufferSpec);

        auto shader = Renderer::GetShaderLibrary()->get("shader");
        m_Pipeline = new MetalPipeline(shader);
    }

    void MetalRenderer::shutDown() {
        delete m_Pipeline;
    }

    void MetalRenderer::beginRenderPass() {
        // resize framebuffer

        // auto fb = ((MetalFrameBuffer*)m_FrameBuffer)->getFrameBuffer();
        // auto colorAttachment = swapChain->getRenderPass()->colorAttachments()->object(0);
        // colorAttachment->setClearColor(m_ClearColor);
        // colorAttachment->setLoadAction(MTL::LoadActionClear);
        // colorAttachment->setStoreAction(MTL::StoreActionStore);
        // colorAttachment->setTexture(swapChain->getCurrentFrameBuffer()->texture());
        
        m_CommandBuffer = MetalDevice::GetInstance()->getCommandQueue()->commandBuffer();

        m_Encoder = m_CommandBuffer->renderCommandEncoder(((MetalFrameBuffer*)m_FrameBuffer)->getFrameBuffer());
    }

    void MetalRenderer::endRenderPass() {
        m_Encoder->endEncoding();
        m_Encoder->release();

        m_CommandBuffer->commit();
        m_CommandBuffer->release();
    }

    void MetalRenderer::setClearColor(const glm::vec4 &color) {
        m_ClearColor = {color.r, color.g, color.b, color.a};
    }

    void MetalRenderer::renderGeometry(Buffer *vertexBuffer,
                                       Buffer *indexBuffer,
                                       Texture2D* texture,
                                       const glm::vec3& color,
                                       uint32_t indexCount,
                                       const glm::mat4 &transform) {
        m_Encoder->setRenderPipelineState(m_Pipeline->getPipelineState());
        m_Encoder->setVertexBuffer((MTL::Buffer*)vertexBuffer->getBuffer(), 0, 0);
        //m_Encoder->setVertexBuffer((MTL::Buffer*)indexBuffer->getBuffer(), 0, 1);
        m_Encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                         indexCount, 
                                         MTL::IndexType::IndexTypeUInt16,
                                         (MTL::Buffer*)indexBuffer->getBuffer(),
                                         0);
        m_Encoder->drawPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle, 
                                  NS::UInteger(0), 
                                  NS::UInteger(3));
    }

    void MetalRenderer::bindUbo(const GlobalUniforms& ubo) {
        
    }

    FrameBuffer* MetalRenderer::getFrameBuffer() {
        return m_FrameBuffer;
    }

    void MetalRenderer::resizeFrameBuffer(uint32_t width, uint32_t height) {

    }
}

