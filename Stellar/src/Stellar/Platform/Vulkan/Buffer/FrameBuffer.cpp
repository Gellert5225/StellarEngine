#include "stlrpch.h"
#include "FrameBuffer.h"

#include "../RenderPass/StandardRenderPass.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

namespace Stellar {

    FrameBuffer::FrameBuffer(const std::vector<VkImageView>& swapChainImageViews,
                             VkExtent2D& swapChainExtent, VkRenderPass renderpass) {
        framebuffers.resize(swapChainImageViews.size());

        for (size_t i = 0; i < swapChainImageViews.size(); i++) {
            VkImageView attachments[] = {
                    swapChainImageViews[i]
            };

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = renderpass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = swapChainExtent.width;
            framebufferInfo.height = swapChainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(VulkanDevice::GetInstance()->logicalDevice(),
                    &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS)
                throw std::runtime_error("Failed to create framebuffer");
        }
    }

    FrameBuffer::~FrameBuffer() {
        for (auto framebuffer: framebuffers)
            vkDestroyFramebuffer(VulkanDevice::GetInstance()->logicalDevice(),
                                 framebuffer, nullptr);
    }

    const std::vector<VkFramebuffer> *FrameBuffer::getFramebuffers() const {
        return &framebuffers;
    }
}