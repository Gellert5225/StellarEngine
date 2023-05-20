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
#include "Stellar/Platform/Metal/Material/MetalMaterial.h"

namespace Stellar {
    void MetalRenderer::init() {
		m_Semaphore = dispatch_semaphore_create(Renderer::MAX_FRAMES_IN_FLIGHT);

        MTL::DepthStencilDescriptor* pDsDesc = MTL::DepthStencilDescriptor::alloc()->init();
		pDsDesc->setDepthCompareFunction(MTL::CompareFunction::CompareFunctionLess);
		pDsDesc->setDepthWriteEnabled(true);

		m_DepthStencilState = MetalDevice::GetInstance()->getDevice()->newDepthStencilState(pDsDesc);

		pDsDesc->release();
    }

    void MetalRenderer::shutDown() {
    }

	void MetalRenderer::beginFrame() {
		
	}

    void MetalRenderer::beginRenderPass(STLR_Ptr<CommandBuffer> commandBuffer, 
										STLR_Ptr<RenderPass> renderPass, 
										bool explicitClear) {
        // resize framebuffer
        m_FrameBuffer = renderPass->getSpecification().targetFramebuffer;
        
        m_CommandBuffer = MetalDevice::GetInstance()->getCommandQueue()->commandBuffer();
		dispatch_semaphore_wait(m_Semaphore, DISPATCH_TIME_FOREVER);
		m_CommandBuffer->addCompletedHandler([&](MTL::CommandBuffer* pCmd) {
			dispatch_semaphore_signal(m_Semaphore);
		});
        m_Encoder = m_CommandBuffer->renderCommandEncoder(m_FrameBuffer.As<MetalFrameBuffer>()->getFrameBuffer());
    }

    void MetalRenderer::endRenderPass(STLR_Ptr<CommandBuffer> commandBuffer) {
        m_Encoder->endEncoding();
        m_CommandBuffer->commit();

        // pool->release();
    }

    void MetalRenderer::setClearColor(const glm::vec4 &color) {
        m_ClearColor = {color.r, color.g, color.b, color.a};
    }

	void MetalRenderer::renderGeometry(STLR_Ptr<CommandBuffer> renderCommandBuffer, 
							STLR_Ptr<Pipeline> pipeline,
							STLR_Ptr<UniformBufferSet> uniformBufferSet, 
							STLR_Ptr<Material> material, 
							STLR_Ptr<Buffer> vertexBuffer, 
							STLR_Ptr<Buffer> indexBuffer, 
							const glm::mat4& transform, 
							uint32_t indexCount) {
		Push p{};
        p.model = transform;

		uint32_t currentFrame = Application::Get().getWindow().getSwapChain()->getCurrentFrameIndex();

		STLR_Ptr<MetalUniformBuffer> uniformBuffer = uniformBufferSet->get(0, 0, currentFrame);

		auto metalPipeline = pipeline.As<MetalPipeline>();
		m_Encoder->setRenderPipelineState(metalPipeline->getPipelineState());
		m_Encoder->setDepthStencilState(m_DepthStencilState);
		m_Encoder->setVertexBuffer(uniformBuffer->getBuffer(), 0, 1);
        m_Encoder->setVertexBuffer((MTL::Buffer*)vertexBuffer->getBuffer(), 0, 0);
        m_Encoder->setVertexBytes(&p, sizeof(Push), 2);

		auto metalMaterial = material.As<MetalMaterial>();
        m_Encoder->setFragmentTextures(metalMaterial->getMetalTextures(), NS::Range(0, 32));
        m_Encoder->drawIndexedPrimitives(MTL::PrimitiveType::PrimitiveTypeTriangle,
                                        indexCount, 
                                        MTL::IndexType::IndexTypeUInt32,
                                        (MTL::Buffer*)indexBuffer->getBuffer(),
                                        0);
	}

    void MetalRenderer::bindUbo(const GlobalUniforms& ubo) {}

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

