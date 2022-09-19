#include "stlrpch.h"

#include "CommandBuffer.h"
#include "RendererAPI.h"

#if defined __linux__ || defined _WIN64
#include "Stellar/Platform/Vulkan/Command/VulkanCommandBuffer.h"
#endif

#include "Stellar/Log.h"

namespace Stellar {

    CommandBuffer* CommandBuffer::Create(uint32_t size) {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan: 
            #if defined __linux__ || defined _WIN64
                return new VulkanCommandBuffer(size);
            #endif
            case RendererAPIType::Metal:
            STLR_CORE_ASSERT(false, "Metal is not yet supported");
                break;
            case RendererAPIType::None:
                break;
        }
        STLR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}
