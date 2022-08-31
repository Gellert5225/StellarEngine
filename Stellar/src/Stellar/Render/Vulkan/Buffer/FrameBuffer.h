#pragma once

#include <Stellar/Core.h>
#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API FrameBuffer {
    public:
        FrameBuffer(const std::vector<VkImageView>&, VkExtent2D&, VkRenderPass);
        ~FrameBuffer();

        [[nodiscard]] const std::vector<VkFramebuffer>* getFramebuffers() const;
        size_t getFramebufferSize() const;
    private:
        std::vector<VkFramebuffer> framebuffers;
    };
}