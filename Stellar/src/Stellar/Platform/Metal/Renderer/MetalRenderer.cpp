#include "stlrpch.h"

#include "MetalRenderer.h"
#include "Stellar/Core/Log.h"
#include "Stellar/Core/Application.h"
#include "Stellar/Platform/Metal/SwapChain/MetalSwapChain.h"
#include "Stellar/Platform/Metal/Device/MetalDevice.h"
#include "Stellar/Platform/Metal/Shader/MetalShader.h"
#include "Stellar/Platform/Metal/Buffer/MetalBuffer.h"
#include "Stellar/Platform/Metal/Buffer/MetalUniformBuffer.h"
#include "Stellar/Platform/Metal/Texture/MetalTexture.h"

namespace Stellar {
    void MetalRenderer::init() {
        // create framebuffer
        FrameBufferSpec framebufferSpec;
        framebufferSpec.width = 1280;
        framebufferSpec.height = 720;
        m_FrameBuffer = FrameBuffer::Create(framebufferSpec);

        auto shader = Renderer::GetShaderLibrary()->get("shader");
        m_Pipeline = STLR_Ptr<MetalPipeline>::Create(shader);

        m_UniformBuffer = UniformBuffer::Create(sizeof(GlobalUniforms), 0);
    }

    void MetalRenderer::shutDown() {
    }

	void MetalRenderer::beginFrame() {
		
	}

    void MetalRenderer::beginRenderPass(STLR_Ptr<CommandBuffer> commandBuffer, 
										STLR_Ptr<RenderPass> renderPass, 
										bool explicitClear) {
        // resize framebuffer

        pool = NS::AutoreleasePool::alloc()->init();
        m_CommandBuffer = MetalDevice::GetInstance()->getCommandQueue()->commandBuffer();
        m_Encoder = m_CommandBuffer->renderCommandEncoder(m_FrameBuffer.As<MetalFrameBuffer>()->getFrameBuffer());
        m_Encoder->setRenderPipelineState(m_Pipeline->getPipelineState());
        m_Encoder->setVertexBuffer(m_UniformBuffer.As<MetalUniformBuffer>()->getBuffer(), 0, 1);
    }

    void MetalRenderer::endRenderPass(STLR_Ptr<CommandBuffer> commandBuffer) {
        m_Encoder->endEncoding();
        m_CommandBuffer->commit();

        pool->release();
        //STLR_CORE_INFO("Command Buffer retain count: {0}", m_CommandBuffer->retainCount());
        //STLR_CORE_INFO("Command Encoder retain count: {0}", m_Encoder->retainCount());
    }

    void MetalRenderer::setClearColor(const glm::vec4 &color) {
        m_ClearColor = {color.r, color.g, color.b, color.a};
    }

    void MetalRenderer::renderGeometry(STLR_Ptr<Buffer> vertexBuffer,
										STLR_Ptr<Buffer> indexBuffer,
										STLR_Ptr<Texture2D> texture,
										const glm::vec4& color,
										uint32_t indexCount,
										const glm::mat4& transform) {
        Push p{};
        p.model = transform;
        //p.color = color;

        m_Encoder->setVertexBuffer((MTL::Buffer*)vertexBuffer->getBuffer(), 0, 0);
        m_Encoder->setVertexBytes(&p, sizeof(Push), 2);
        m_Encoder->setFragmentTexture(texture.As<MetalTexture>()->getTexture(), 0);
        m_Encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                         indexCount, 
                                         MTL::IndexType::IndexTypeUInt16,
                                         (MTL::Buffer*)indexBuffer->getBuffer(),
                                         0);
    }

	void MetalRenderer::renderGeometry(STLR_Ptr<CommandBuffer> renderCommandBuffer, 
							STLR_Ptr<Pipeline> pipeline,
							STLR_Ptr<UniformBufferSet> uniformBufferSet, 
							STLR_Ptr<Material> material, 
							STLR_Ptr<Buffer> vertexBuffer, 
							STLR_Ptr<Buffer> indexBuffer, 
							const glm::mat4& transform, 
							uint32_t indexCount ) {

	}

    void MetalRenderer::bindUbo(const GlobalUniforms& ubo) {
        // reinterpret_cast<GlobalUniforms*>(((MTL::Buffer*)m_UniformBuffer->getBuffer())->contents())->viewProjection = ubo.viewProjection;
        // ((MetalBuffer*)m_UniformBuffer)->didModifyrange();
    }

    STLR_Ptr<FrameBuffer> MetalRenderer::getFrameBuffer() {
        return m_FrameBuffer;
    }

    void MetalRenderer::resizeFrameBuffer(uint32_t width, uint32_t height) {

    }

    void MetalRenderer::renderGrid(STLR_Ptr<Buffer> vertexBuffer,
                                STLR_Ptr<Buffer> indexBuffer,
                                uint32_t indexCount) {
        
    }
}

