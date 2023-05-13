#pragma once

#include "Stellar/Core/Core.h"

#include "Stellar/Renderer/Pipeline.h"
#include "Stellar/Platform/Metal/Shader/MetalShader.h"
#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {
    class STLR_API MetalPipeline : public Pipeline {
    public:
        MetalPipeline(STLR_Ptr<Shader> shader);
		MetalPipeline(PipelineSpecification spec);
        ~MetalPipeline();

		virtual PipelineSpecification& getSpecification() override { return m_Specification; };
		virtual const PipelineSpecification& getSpecification() const override { return m_Specification; };

		virtual void invalidate() override;
		virtual void setUniformBuffer(STLR_Ptr<Buffer> uniformBuffer, uint32_t binding, uint32_t set = 0) override;

		// TEMP: remove this when render command buffers are a thing
		virtual void bind() override;

        MTL::RenderPipelineState* getPipelineState();
    private:
        MTL::RenderPipelineState* m_PipelineState;
		PipelineSpecification m_Specification;
    };
}  
