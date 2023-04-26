#include "stlrpch.h"
#include "VulkanRenderPass.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"

namespace Stellar {
	VulkanRenderPass::VulkanRenderPass(const RenderPassSpecification& spec) {
		this->m_Specification = spec;
	}
	
	VulkanRenderPass::VulkanRenderPass(const VkFormat &swapChainImageFormat) {
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(VulkanDevice::GetInstance()->logicalDevice(),
							&renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	VulkanRenderPass::VulkanRenderPass(const std::vector<VkAttachmentDescription>& attachmentDescriptions, const std::vector<VkAttachmentReference>& colorAttachmentReferences, VkAttachmentReference depthAttachmentReference) {
		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = uint32_t(colorAttachmentReferences.size());
		subpassDescription.pColorAttachments = colorAttachmentReferences.data();
		subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;

		// TODO: do we need these?
		// Use subpass dependencies for layout transitions
		std::vector<VkSubpassDependency> dependencies;

		{
			VkSubpassDependency& depedency = dependencies.emplace_back();
			depedency.srcSubpass = VK_SUBPASS_EXTERNAL;
			depedency.dstSubpass = 0;
			depedency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			depedency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			depedency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			depedency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			depedency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		}
		{
			VkSubpassDependency& depedency = dependencies.emplace_back();
			depedency.srcSubpass = 0;
			depedency.dstSubpass = VK_SUBPASS_EXTERNAL;
			depedency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			depedency.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			depedency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			depedency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			depedency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		}

		{
			VkSubpassDependency& depedency = dependencies.emplace_back();
			depedency.srcSubpass = VK_SUBPASS_EXTERNAL;
			depedency.dstSubpass = 0;
			depedency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			depedency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			depedency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
			depedency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			depedency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		}

		{
			VkSubpassDependency& depedency = dependencies.emplace_back();
			depedency.srcSubpass = 0;
			depedency.dstSubpass = VK_SUBPASS_EXTERNAL;
			depedency.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			depedency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			depedency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			depedency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			depedency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		}

		// Create the actual renderpass
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptions.size());
		renderPassInfo.pAttachments = attachmentDescriptions.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

		auto device = VulkanDevice::GetInstance()->logicalDevice();
		VK_CHECK_RESULT(vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_RenderPass));
	}

	VulkanRenderPass::~VulkanRenderPass() {
		vkDestroyRenderPass(VulkanDevice::GetInstance()->logicalDevice(), m_RenderPass, nullptr);
	}

	VkRenderPass VulkanRenderPass::getVkRenderPass() const {
		return m_RenderPass;
	}
}