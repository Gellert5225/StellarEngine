#include "FrameBuffer.h"

#include "Stellar/Renderer/RendererAPI.h"
#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Buffer/VulkanFrameBuffer.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Buffer/MetalFrameBuffer.h"
#endif
#include "Stellar/Core/Log.h"

namespace Stellar {
	STLR_Ptr<FrameBuffer> FrameBuffer::Create(const FrameBufferSpec& spec) {
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan:
				#if defined __linux__ || defined _WIN64
					return STLR_Ptr<VulkanFrameBuffer>::Create(spec);
				#endif
			case RendererAPIType::Metal:
				#if defined __APPLE__
					return new MetalFrameBuffer(spec);
				#endif
			case RendererAPIType::None:
				break;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}