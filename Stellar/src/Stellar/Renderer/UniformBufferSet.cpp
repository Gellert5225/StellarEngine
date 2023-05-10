#include "stlrpch.h"
#include "UniformBufferSet.h"

#include "Stellar/Renderer/RendererAPI.h"
#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Buffer/VulkanUniformBufferSet.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Buffer/MetalBuffer.h"
#endif
#include "Stellar/Core/Log.h"

namespace Stellar {
	STLR_Ptr<UniformBufferSet> UniformBufferSet::Create(uint32_t frames) {
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan:
			#if defined(__linux__) || defined(_WIN64)
				return STLR_Ptr<VulkanUniformBufferSet>::Create(frames);
			#endif
			case RendererAPIType::Metal:
			#if defined(__APPLE__)
				return new MetalShader(filePath);
			#endif
			case RendererAPIType::None:
				break;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}