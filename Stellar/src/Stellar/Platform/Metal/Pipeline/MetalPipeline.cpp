#include "stlrpch.h"
#include "MetalPipeline.h"

#include "Stellar/Platform/Metal/Device/MetalDevice.h"
#include "Stellar/Core/Log.h"

namespace Stellar {
    MetalPipeline::MetalPipeline(STLR_Ptr<Shader> shader) {
        MTL::Function* vertexFn = shader.As<MetalShader>()->getLibrary()->newFunction(NS::String::string("vertexMain", NS::StringEncoding::UTF8StringEncoding));
        MTL::Function* fragFn = shader.As<MetalShader>()->getLibrary()->newFunction(NS::String::string("fragmentMain", NS::StringEncoding::UTF8StringEncoding));

        MTL::RenderPipelineDescriptor* descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        descriptor->setVertexFunction(vertexFn);
        descriptor->setFragmentFunction(fragFn);
        descriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);

        NS::Error* error = nullptr; 
        m_PipelineState =  MetalDevice::GetInstance()->getDevice()->newRenderPipelineState(descriptor, &error);
        STLR_CORE_ASSERT(m_PipelineState,  error->localizedDescription()->utf8String());

        descriptor->release();
        vertexFn->release();
        fragFn->release();
    }

	MetalPipeline::MetalPipeline(PipelineSpecification spec) : m_Specification(spec) {
		auto shader = spec.shader.As<MetalShader>();
		MTL::Function* vertexFn = shader->getLibrary()->newFunction(NS::String::string("vertexMain", NS::StringEncoding::UTF8StringEncoding));
        MTL::Function* fragFn = shader->getLibrary()->newFunction(NS::String::string("fragmentMain", NS::StringEncoding::UTF8StringEncoding));

        MTL::RenderPipelineDescriptor* descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        descriptor->setVertexFunction(vertexFn);
        descriptor->setFragmentFunction(fragFn);
		descriptor->setDepthAttachmentPixelFormat(MTL::PixelFormat::PixelFormatDepth32Float);
        descriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);
		descriptor->colorAttachments()->object(0)->setBlendingEnabled(true);
		descriptor->colorAttachments()->object(0)->setRgbBlendOperation(MTL::BlendOperation::BlendOperationAdd);
		descriptor->colorAttachments()->object(0)->setAlphaBlendOperation(MTL::BlendOperation::BlendOperationAdd);
		descriptor->colorAttachments()->object(0)->setSourceAlphaBlendFactor(MTL::BlendFactor::BlendFactorSourceAlpha);
		descriptor->colorAttachments()->object(0)->setSourceRGBBlendFactor(MTL::BlendFactor::BlendFactorSourceAlpha);
		descriptor->colorAttachments()->object(0)->setDestinationAlphaBlendFactor(MTL::BlendFactor::BlendFactorOneMinusSourceAlpha);
		descriptor->colorAttachments()->object(0)->setDestinationRGBBlendFactor(MTL::BlendFactor::BlendFactorOneMinusSourceAlpha);

        NS::Error* error = nullptr; 
        m_PipelineState =  MetalDevice::GetInstance()->getDevice()->newRenderPipelineState(descriptor, &error);
        STLR_CORE_ASSERT(m_PipelineState,  error->localizedDescription()->utf8String());

        descriptor->release();
        vertexFn->release();
        fragFn->release();
	}

	void MetalPipeline::invalidate() {

	}

	void MetalPipeline::setUniformBuffer(STLR_Ptr<Buffer> uniformBuffer, uint32_t binding, uint32_t set) {

	}

	void MetalPipeline::bind() {

	}

    MetalPipeline::~MetalPipeline() {
        m_PipelineState->release();
    }

    MTL::RenderPipelineState* MetalPipeline::getPipelineState() {
        return m_PipelineState;
    }
}
