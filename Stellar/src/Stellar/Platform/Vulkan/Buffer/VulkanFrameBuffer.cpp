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

		m_AttachmentImages.clear();

		bool createImages = m_AttachmentImages.empty();
		std::vector<VkAttachmentDescription> attachmentDescriptions;
		std::vector<VkAttachmentReference> colorAttachmentReferences;
		VkAttachmentReference depthAttachmentReference;

		uint32_t attachmentIndex = 0;
		for (auto attachment : m_Spec.attachments.attachments) {
			if (Utils::IsDepthFormat(attachment.format)) { // depth attachment
				ImageSpecification imageSpec;
				imageSpec.format = attachment.format;
				imageSpec.usage = ImageUsage::Attachment;
				imageSpec.width = m_Width;
				imageSpec.height = m_Height;
				imageSpec.mips = 1;
				m_DepthAttachmentImage = Image2D::Create(imageSpec);

				m_DepthAttachmentImage->invalidate();

				VkAttachmentDescription& attachmentDescription = attachmentDescriptions.emplace_back();
				attachmentDescription.flags = 0;
				attachmentDescription.format = Utils::VulkanImageFormat(attachment.format);
				attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
				attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				if (attachment.format == ImageFormat::DEPTH24STENCIL8 || true) {
					attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // TODO: if not sampling
					attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL; // TODO: if sampling
					depthAttachmentReference = { attachmentIndex, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };
				} else {
					attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL; // TODO: if not sampling
					attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL; // TODO: if sampling
					depthAttachmentReference = { attachmentIndex, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL };
				}
			} else {
				STLR_Ptr<Image2D> colorAttachment;
				if (createImages) {
					ImageSpecification imageSpec;
					imageSpec.format = attachment.format;
					imageSpec.usage = ImageUsage::Attachment;
					imageSpec.width = m_Width;
					imageSpec.height = m_Height;
					imageSpec.mips = 1;
					colorAttachment = m_AttachmentImages.emplace_back(Image2D::Create(imageSpec));
					colorAttachment->invalidate();
				} else {
					STLR_Ptr<Image2D> image = m_AttachmentImages[attachmentIndex];
					ImageSpecification& spec = image->GetSpecification();
					spec.width = m_Width;
					spec.height = m_Height;
					colorAttachment = image;
					colorAttachment->invalidate();
				}

				VkAttachmentDescription& attachmentDescription = attachmentDescriptions.emplace_back();
				attachmentDescription.flags = 0;
				attachmentDescription.format = Utils::VulkanImageFormat(attachment.format);
				attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
				attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // TODO: if sampling, needs to be store (otherwise DONT_CARE is fine)
				attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

				colorAttachmentReferences.emplace_back(VkAttachmentReference { attachmentIndex, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
			}
			attachmentIndex++;
		}

		// create render pass
		m_RenderPass = STLR_Ptr<VulkanRenderPass>::Create(attachmentDescriptions, colorAttachmentReferences, depthAttachmentReference);

		std::vector<VkImageView> attachments(m_AttachmentImages.size());

		for (uint32_t i = 0; i < m_AttachmentImages.size(); i++) {
			VulkanImageInfo* info = (VulkanImageInfo*)((VulkanImage2D*)m_AttachmentImages[i].raw())->getImageInfo();
			attachments[i] = info->imageView;
		}

		if (m_DepthAttachmentImage) {
			VulkanImageInfo* depthInfo = (VulkanImageInfo*)(VulkanImage2D*)m_DepthAttachmentImage.raw()->getImageInfo();
			attachments.emplace_back(depthInfo->imageView);
		}

		// std::array<VkImageView, 2> attachments = {
		// 	info->imageView,
		// 	depthInfo->imageView
		// };
		
		// create framebuffer
		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass->getVkRenderPass();
		framebufferInfo.attachmentCount = attachments.size();
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

			for (STLR_Ptr<Image2D> image : m_AttachmentImages) {
				image->release();
			}
			m_DepthAttachmentImage->release();
		}
	}

	VulkanFrameBuffer::~VulkanFrameBuffer() {
		release();
	}

	VkFramebuffer VulkanFrameBuffer::getFramebuffer() const {
		return m_Framebuffer;
	}

	VkRenderPass VulkanFrameBuffer::getRenderPass() const {
		return m_RenderPass->getVkRenderPass();
	}
}