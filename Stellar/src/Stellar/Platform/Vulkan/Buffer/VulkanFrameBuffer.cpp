#include "stlrpch.h"
#include "VulkanFrameBuffer.h"

#include "Stellar/Platform/Vulkan/RenderPass/StandardRenderPass.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"

namespace Stellar {

    VulkanFrameBuffer::VulkanFrameBuffer(const FrameBufferSpec& spec): m_Spec(spec) {
        invalidate();
    }

    void VulkanFrameBuffer::invalidate() {
        auto device = VulkanDevice::GetInstance()->logicalDevice();

        // create render pass
        StandardRenderPass* renderPass = new StandardRenderPass(VK_FORMAT_R32G32B32A32_SFLOAT);
        // create image
        
        // create framebuffer
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass->getVkRenderPass();
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.width = m_Spec.width;
        framebufferInfo.height = m_Spec.height;
        framebufferInfo.layers = 1;

        VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_Framebuffer));
    }

    VulkanFrameBuffer::~VulkanFrameBuffer() {
        vkDestroyFramebuffer(VulkanDevice::GetInstance()->logicalDevice(), m_Framebuffer, nullptr);
    }

    const std::vector<VkFramebuffer> *VulkanFrameBuffer::getFramebuffers() const {
        // return &framebuffers;
    }
}