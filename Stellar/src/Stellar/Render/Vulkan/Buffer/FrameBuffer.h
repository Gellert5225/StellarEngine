#pragma once

#include <Stellar/Core.h>
#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API FrameBuffer {
    public:
        FrameBuffer(const std::vector<VkImageView>& swapChainImageViews,
                    VkExtent2D& swapChainExtent,
                    VkRenderPass renderpass);
        ~FrameBuffer();

        [[nodiscard]] const std::vector<VkFramebuffer>* getFramebuffers() const;
        size_t getFramebufferSize() const;
    private:
        std::vector<VkFramebuffer> framebuffers;
    };
}