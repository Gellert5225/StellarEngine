#include "Buffer.h"

#include "Stellar/Renderer/RendererAPI.h"
#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Buffer/VulkanBuffer.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Buffer/MetalBuffer.h"
#endif
#include "Stellar/Core/Log.h"

namespace Stellar {

	// Vertex and Index Buffer
	STLR_Ptr<Buffer> Buffer::Create(BufferType type, uint64_t size, const void *data) {
		STLR_CORE_ASSERT(type != BufferType::Uniform, "Please use Buffer::Create(uint32_t size, uint32_t binding) to create uniform buffer");
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan:
			#if defined(__linux__) || defined(_WIN64)
				switch (type) {
					case BufferType::Vertex:
						return STLR_Ptr<VulkanBuffer>::Create(size,
												VK_BUFFER_USAGE_TRANSFER_DST_BIT |
												VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
												VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
												data);
					case BufferType::Index:
						return STLR_Ptr<VulkanBuffer>::Create(size,
												VK_BUFFER_USAGE_TRANSFER_DST_BIT |
												VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
												VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
												data);
				}
			#endif
			case RendererAPIType::Metal:
			#if defined(__APPLE__)
				return new MetalBuffer(size, data);
			#endif
			case RendererAPIType::None:
				break;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	// Uniform Buffer
	STLR_Ptr<Buffer> Buffer::Create(uint32_t size, uint32_t binding) {
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan:
			#if defined(__linux__) || defined(_WIN64)
				return STLR_Ptr<VulkanUniformBuffer>::Create(size, binding);
			#endif
			case RendererAPIType::Metal:
			#if defined(__APPLE__)
				// return new MetalBuffer(size, data);
			#endif
			case RendererAPIType::None:
				break;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}
