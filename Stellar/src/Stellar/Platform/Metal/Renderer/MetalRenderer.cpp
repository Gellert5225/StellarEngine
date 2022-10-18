#include "stlrpch.h"

#include "MetalRenderer.h"
#include "Stellar/Core/Log.h"
#include "Stellar/Core/Application.h"
#include "Stellar/Platform/Metal/SwapChain/MetalSwapChain.h"
#include "Stellar/Platform/Metal/Device/MetalDevice.h"
#include "Stellar/Platform/Metal/Shader/MetalShader.h"
#include "Stellar/Platform/Metal/Buffer/MetalBuffer.h"
#include "Stellar/Platform/Metal/Texture/MetalTexture.h"

namespace Stellar {
    void MetalRenderer::init() {
        // create framebuffer
        FrameBufferSpec framebufferSpec;
        framebufferSpec.width = 1280;
        framebufferSpec.height = 720;
        m_FrameBuffer = FrameBuffer::Create(framebufferSpec);

        auto shader = Renderer::GetShaderLibrary()->get("shader");
        m_Pipeline = new MetalPipeline(shader);

        m_UniformBuffer = Buffer::Create(BufferType::Uniform, sizeof(GlobalUniforms));
    }

    void MetalRenderer::shutDown() {
        delete m_Pipeline;
        delete m_UniformBuffer;
    }

    void MetalRenderer::beginRenderPass() {
        // resize framebuffer

        pool = NS::AutoreleasePool::alloc()->init();
        m_CommandBuffer = MetalDevice::GetInstance()->getCommandQueue()->commandBuffer();

        m_Encoder = m_CommandBuffer->renderCommandEncoder(((MetalFrameBuffer*)m_FrameBuffer)->getFrameBuffer());
        m_Encoder->setRenderPipelineState(m_Pipeline->getPipelineState());
        m_Encoder->setVertexBuffer((MTL::Buffer*)m_UniformBuffer->getBuffer(), 0, 1);
    }

    void MetalRenderer::endRenderPass() {
        m_Encoder->endEncoding();
        m_CommandBuffer->commit();

        pool->release();
        //STLR_CORE_INFO("Command Buffer retain count: {0}", m_CommandBuffer->retainCount());
        //STLR_CORE_INFO("Command Encoder retain count: {0}", m_Encoder->retainCount());
    }

    void MetalRenderer::setClearColor(const glm::vec4 &color) {
        m_ClearColor = {color.r, color.g, color.b, color.a};
    }

    void MetalRenderer::renderGeometry(Buffer *vertexBuffer,
                                       Buffer *indexBuffer,
                                       Texture2D* texture,
                                       const glm::vec4& color,
                                       uint32_t indexCount,
                                       const glm::mat4 &transform) {
        Push p{};
        p.model = transform;
        p.color = color;

        m_Encoder->setVertexBuffer((MTL::Buffer*)vertexBuffer->getBuffer(), 0, 0);
        m_Encoder->setVertexBytes(&p, sizeof(Push), 2);
        m_Encoder->setFragmentTexture(((MetalTexture*)texture)->getTexture(), 0);
        m_Encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                         indexCount, 
                                         MTL::IndexType::IndexTypeUInt16,
                                         (MTL::Buffer*)indexBuffer->getBuffer(),
                                         0);
    }

    void MetalRenderer::bindUbo(const GlobalUniforms& ubo) {
        reinterpret_cast<GlobalUniforms*>(((MTL::Buffer*)m_UniformBuffer->getBuffer())->contents())->viewProjection = ubo.viewProjection;
        ((MetalBuffer*)m_UniformBuffer)->didModifyrange();
    }

    FrameBuffer* MetalRenderer::getFrameBuffer() {
        return m_FrameBuffer;
    }

    void MetalRenderer::resizeFrameBuffer(uint32_t width, uint32_t height) {

    }
}

