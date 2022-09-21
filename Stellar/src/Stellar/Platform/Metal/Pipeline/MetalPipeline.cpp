#include "stlrpch.h"
#include "MetalPipeline.h"

#include "Stellar/Platform/Metal/Device/MetalDevice.h"
#include "Stellar/Log.h"

namespace Stellar {
    MetalPipeline::MetalPipeline(MetalShader* shader) {
        MTL::Function* vertexFn = shader->getLibrary()->newFunction(NS::String::string("vertexMain", NS::StringEncoding::UTF8StringEncoding));
        MTL::Function* fragFn = shader->getLibrary()->newFunction(NS::String::string("fragmentMain", NS::StringEncoding::UTF8StringEncoding));

        MTL::RenderPipelineDescriptor* descriptor = MTL::RenderPipelineDescriptor::alloc()->init();
        descriptor->setVertexFunction(vertexFn);
        descriptor->setFragmentFunction(fragFn);
        descriptor->colorAttachments()->object(0)->setPixelFormat(MTL::PixelFormat::PixelFormatBGRA8Unorm);

        NS::Error* error = nullptr; 
        m_PipelineState =  MetalDevice::GetInstance()->getDevice()->newRenderPipelineState(descriptor, &error);
        STLR_CORE_ASSERT(m_PipelineState,  error->localizedDescription()->utf8String());

        vertexFn->release();
        fragFn->release();
    }

    MetalPipeline::~MetalPipeline() {
        m_PipelineState->release();
    }

    MTL::RenderPipelineState* MetalPipeline::getPipelineState() {
        return m_PipelineState;
    }
}
