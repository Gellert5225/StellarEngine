 #include "stlrpch.h"
#include "VulkanFrameBuffer.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/Image/VulkanImage.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"

namespace Stellar {

    VulkanFrameBuffer::VulkanFrameBuffer(const FrameBufferSpec& spec) {
        m_Width = spec.width;
        m_Height = spec.height;
        m_Spec = spec;
        invalidate();
    }

    void VulkanFrameBuffer::invalidate() {
        auto device = VulkanDevice::GetInstance()->logicalDevice();

        VkFormat imageFormat;
        VkFormat depthFormat;

        for (auto attachment : m_Spec.attachments) {
            if (Utils::IsDepthFormat(attachment)) { // depth attachment
                depthFormat = Utils::VulkanImageFormat(attachment);
                ImageSpecification imageSpec;
                imageSpec.format = attachment;
                imageSpec.usage = ImageUsage::Attachment;
                imageSpec.width = m_Width;
                imageSpec.height = m_Height;
                imageSpec.mips = 1;
                m_DepthAttachmentImage = Image2D::Create(imageSpec);

                m_DepthAttachmentImage->invalidate();
            } else {
                imageFormat = Utils::VulkanImageFormat(attachment);
                ImageSpecification imageSpec;
                imageSpec.format = attachment;
                imageSpec.usage = ImageUsage::Attachment;
                imageSpec.width = m_Width;
                imageSpec.height = m_Height;
                imageSpec.mips = 1;
                m_AttachmentImage = Image2D::Create(imageSpec);

                m_AttachmentImage->invalidate();
            }
        }

        // create render pass
        m_RenderPass = new StandardRenderPass(imageFormat, depthFormat);

        Ref<VulkanImage2D> image = std::static_pointer_cast<VulkanImage2D>(m_AttachmentImage);
        VulkanImageInfo* info = (VulkanImageInfo*)image->getImageInfo();

       	Ref<VulkanImage2D> depthImage = std::static_pointer_cast<VulkanImage2D>(m_DepthAttachmentImage);
        VulkanImageInfo* depthInfo = (VulkanImageInfo*)depthImage->getImageInfo();

        std::array<VkImageView, 2> attachments = {
            info->imageView,
            depthInfo->imageView
        };
        
        // create framebuffer
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_RenderPass->getVkRenderPass();
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.width = m_Width;
        framebufferInfo.height = m_Height;
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.layers = 1;

        VK_CHECK_RESULT(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &m_Framebuffer));
    }

    void VulkanFrameBuffer::resize(uint32_t width, uint32_t height) {
        release();
        m_Width = width;
        m_Height = height;
        m_Spec.width = width;
        m_Spec.height = height;
        invalidate();
    }

    void VulkanFrameBuffer::release() {
        if (m_Framebuffer) {
            auto device = VulkanDevice::GetInstance()->logicalDevice();
            vkDestroyFramebuffer(device, m_Framebuffer, nullptr);
            m_AttachmentImage->release();
            m_DepthAttachmentImage->release();
            delete m_RenderPass;
        }
    }

    VulkanFrameBuffer::~VulkanFrameBuffer() {
        vkDestroyFramebuffer(VulkanDevice::GetInstance()->logicalDevice(), m_Framebuffer, nullptr);
        delete m_RenderPass;
    }

    VkFramebuffer VulkanFrameBuffer::getFramebuffer() const {
        return m_Framebuffer;
    }

    VkRenderPass VulkanFrameBuffer::getRenderPass() const {
        return m_RenderPass->getVkRenderPass();
    }
}