#include "stlrpch.h"

#include "RenderPass.h"

#include "Stellar/Renderer/RendererAPI.h"

#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/RenderPass/VulkanRenderPass.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/RenderPass/MetalRenderPass.h"
#endif

namespace Stellar {
	STLR_Ptr<RenderPass> RenderPass::Create(const RenderPassSpecification& spec) {
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan: 
			#if defined(__linux__) || defined(_WIN64)
				return STLR_Ptr<VulkanRenderPass>::Create(spec);
			#endif
			case RendererAPIType::Metal: 
			#if defined(__APPLE__)
				return STLR_Ptr<MetalRenderPass>::Create(spec);
			#endif
			case RendererAPIType::None:
				break;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}