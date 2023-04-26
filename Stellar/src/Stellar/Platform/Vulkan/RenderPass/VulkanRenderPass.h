#pragma once

#include "Stellar/Renderer/RenderPass.h"
#include "Stellar/Core/Core.h"

#include <vector>
#include <vulkan/vulkan.h>

namespace Stellar {
	class VulkanRenderPass : public RenderPass {
	public:
		explicit VulkanRenderPass(const VkFormat& swapChainImageFormat);
		explicit VulkanRenderPass(const RenderPassSpecification& spec);
		VulkanRenderPass(const std::vector<VkAttachmentDescription>& attachmentDescriptions, const std::vector<VkAttachmentReference>& colorAttachmentReferences, VkAttachmentReference depthAttachmentReference);

		virtual ~VulkanRenderPass();

		VkRenderPass getVkRenderPass() const;
	private:
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
	};
}