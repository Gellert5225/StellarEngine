#include "Buffer.h"

#include "Stellar/Renderer/RendererAPI.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanBuffer.h"
#include "Stellar/Log.h"

namespace Stellar {

    /// Vertex Buffer
    VertexBuffer* VertexBuffer::Create(uint64_t size, const void *data) {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan:
                if (data) {
                    return new VulkanVertexBuffer(size,
                                                  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                  data);
                } else {
                    return new VulkanVertexBuffer(size,
                                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                                  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
                }

            case RendererAPIType::Metal:
                STLR_CORE_ASSERT(false, "Metal is not yet supported");
                break;
            case RendererAPIType::None:
                break;
        }
        STLR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

    /// Index Buffer
    IndexBuffer* IndexBuffer::Create(uint64_t size, const void *data) {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan:
                if (data) {
                    return new VulkanIndexBuffer(size,
                                                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                                 data);
                } else {
                    return new VulkanIndexBuffer(size,
                                                 VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                                 VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
                }

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
