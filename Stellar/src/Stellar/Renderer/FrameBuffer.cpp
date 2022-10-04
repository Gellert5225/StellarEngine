#include "FrameBuffer.h"

#include "Stellar/Renderer/RendererAPI.h"
#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Buffer/VulkanFrameBuffer.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Buffer/MetalBuffer.h"
#endif
#include "Stellar/Core/Log.h"

namespace Stellar {
    FrameBuffer* FrameBuffer::Create(const FrameBufferSpec& spec) {
        switch (RendererAPI::Current()) {
            case RendererAPIType::Vulkan:
                #if defined __linux__ || defined _WIN64
                    return new VulkanFrameBuffer(spec);
                #endif
            case RendererAPIType::Metal:
                STLR_CORE_ASSERT(false, "Metal is not supported");
            case RendererAPIType::None:
                break;
        }
        STLR_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}