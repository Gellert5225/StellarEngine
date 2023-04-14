#include "stlrpch.h"

#include "RenderPass.h"

#include "Stellar/Renderer/RendererAPI.h"

#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/RenderPass/VulkanRenderPass.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Shader/MetalShader.h"
#endif

namespace Stellar {
	STLR_Ptr<RenderPass> RenderPass::Create(const RenderPassSpecification& spec) {
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan: return STLR_Ptr<VulkanRenderPass>::Create(spec);
		}
		return nullptr;
	}
}