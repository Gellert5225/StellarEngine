#pragma once

#include "Stellar/Core.h"

#include "Stellar/Platform/Metal/Shader/MetalShader.h"
#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {
    class STLR_API MetalPipeline {
    public:
        MetalPipeline(MetalShader* shader);
        ~MetalPipeline();

        MTL::RenderPipelineState* getPipelineState();
    private:
        MTL::RenderPipelineState* m_PipelineState;
    };
}  
