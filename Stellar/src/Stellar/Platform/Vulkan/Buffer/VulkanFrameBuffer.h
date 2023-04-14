#pragma once

#include "Stellar/Renderer/FrameBuffer.h"
#include "Stellar/Renderer/Image.h"

#include "Stellar/Platform/Vulkan/RenderPass/VulkanRenderPass.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Stellar {
	class VulkanFrameBuffer : public FrameBuffer {
	public:
		VulkanFrameBuffer(const FrameBufferSpec& spec);
		~VulkanFrameBuffer();

		void resize(uint32_t width, uint32_t height) override;

		void invalidate();
		void release();
		STLR_Ptr<Image2D> getAttachmentImage() override { return m_AttachmentImages[0]; }
		STLR_Ptr<Image2D> getDepthAttachmentImage() override { return m_DepthAttachmentImage; }
		const FrameBufferSpec& getSpecification() const override { return m_Spec; };

		[[nodiscard]] VkFramebuffer getFramebuffer() const;
		[[nodiscard]] VkRenderPass getRenderPass() const;
		[[nodiscard]] size_t getFramebufferSize() const;
	private:
		VkFramebuffer m_Framebuffer;
		STLR_Ptr<VulkanRenderPass> m_RenderPass;
	};
}