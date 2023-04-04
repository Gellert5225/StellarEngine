#pragma once

#include "Stellar/Renderer/FrameBuffer.h"
#include "Stellar/Renderer/Image.h"

#include "Stellar/Platform/Vulkan/RenderPass/StandardRenderPass.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace Stellar {
	class STLR_API VulkanFrameBuffer : public FrameBuffer {
	public:
		VulkanFrameBuffer(const FrameBufferSpec& spec);
		~VulkanFrameBuffer();

		void resize(uint32_t width, uint32_t height) override;

		void invalidate();
		void release();
		Ref<Image2D> getAttachmentImage() override { return m_AttachmentImage; }
		Ref<Image2D> getDepthAttachmentImage() override { return m_DepthAttachmentImage; }
		const FrameBufferSpec& getSpecification() const override { return m_Spec; };

		[[nodiscard]] VkFramebuffer getFramebuffer() const;
		[[nodiscard]] VkRenderPass getRenderPass() const;
		[[nodiscard]] size_t getFramebufferSize() const;
	private:
		VkFramebuffer m_Framebuffer;
		Ref<StandardRenderPass> m_RenderPass;
	};
}