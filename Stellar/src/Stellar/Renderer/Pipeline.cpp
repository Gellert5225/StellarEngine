#include "stlrpch.h"
#include "Pipeline.h"

#include "Stellar/Renderer/RendererAPI.h"

#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Pipeline/VulkanPipeline.h"
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
				//TODO: Implement Metal Pipeline
			#endif
			case RendererAPIType::None:
				break;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}