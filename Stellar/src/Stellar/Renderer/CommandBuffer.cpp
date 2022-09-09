#include "stlrpch.h"

#include "CommandBuffer.h"
#include "RendererAPI.h"

#include "Stellar/Platform/Vulkan/Command/VulkanCommandBuffer.h"

#include "Stellar/Log.h"

namespace Stellar {

    CommandBuffer* CommandBuffer::Create(uint32_t size) {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan: return new VulkanCommandBuffer(size);
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
