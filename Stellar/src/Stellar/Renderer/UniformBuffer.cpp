#include "stlrpch.h"
#include "UniformBuffer.h"

#include "Stellar/Renderer/RendererAPI.h"
#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Buffer/VulkanUniformBuffer.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Buffer/MetalUniformBuffer.h"
#endif
#include "Stellar/Core/Log.h"

namespace Stellar {
	STLR_Ptr<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding) {
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan:
			#if defined(__linux__) || defined(_WIN64)
				return STLR_Ptr<VulkanUniformBuffer>::Create(size, binding);
			#endif
			case RendererAPIType::Metal:
			#if defined(__APPLE__)
				return STLR_Ptr<MetalUniformBuffer>::Create(size, binding);
			#endif
			case RendererAPIType::None:
				break;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}