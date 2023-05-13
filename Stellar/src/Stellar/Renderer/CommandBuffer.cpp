#include "stlrpch.h"

#include "CommandBuffer.h"
#include "RendererAPI.h"

#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Command/VulkanCommandBuffer.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/CommandBuffer/MetalCommandBuffer.h"
#endif

#include "Stellar/Core/Log.h"

namespace Stellar {

	STLR_Ptr<CommandBuffer> CommandBuffer::Create(uint32_t size) {
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan: 
			#if defined __linux__ || defined _WIN64
				return STLR_Ptr<VulkanCommandBuffer>::Create(size);
			#endif
			case RendererAPIType::Metal:
			#if defined __APPLE__
				return STLR_Ptr<MetalCommandBuffer>::Create(size);
				break;
			#endif
			case RendererAPIType::None:
				break;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}
