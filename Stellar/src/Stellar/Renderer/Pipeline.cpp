#include "stlrpch.h"
#include "Pipeline.h"

#include "Stellar/Renderer/RendererAPI.h"

#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Pipeline/VulkanPipeline.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Pipeline/MetalPipeline.h"
#endif

namespace Stellar {
    STLR_Ptr<Pipeline> Pipeline::Create(const PipelineSpecification& spec) {
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan:
			#if defined(__linux__) || defined(_WIN64)
				return STLR_Ptr<VulkanPipeline>::Create(spec);
			#endif
			case RendererAPIType::Metal:
			#if defined(__APPLE__)
				return STLR_Ptr<MetalPipeline>::Create(spec);
			#endif
			case RendererAPIType::None:
				break;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}