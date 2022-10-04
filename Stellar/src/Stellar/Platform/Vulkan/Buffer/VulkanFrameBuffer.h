#pragma once

#include "Stellar/Renderer/FrameBuffer.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Stellar {
    class STLR_API VulkanFrameBuffer : public FrameBuffer {
    public:
        VulkanFrameBuffer(const FrameBufferSpec& spec);
        ~VulkanFrameBuffer();

        void invalidate();

        [[nodiscard]] const std::vector<VkFramebuffer>* getFramebuffers() const;
        size_t getFramebufferSize() const;
        const FrameBufferSpec& getSpecification() const { return m_Spec; };
    private:
        VkFramebuffer m_Framebuffer;
        FrameBufferSpec m_Spec;
    };
}