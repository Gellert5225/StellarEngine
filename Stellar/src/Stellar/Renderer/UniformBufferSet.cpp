#include "stlrpch.h"
#include "UniformBufferSet.h"

#include "Stellar/Renderer/RendererAPI.h"
#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Buffer/VulkanBuffer.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Buffer/MetalBuffer.h"
#endif
#include "Stellar/Core/Log.h"

namespace Stellar {
	
}