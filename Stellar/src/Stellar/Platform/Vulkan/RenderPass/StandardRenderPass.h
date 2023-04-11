#pragma once

#include "Stellar/Core/Core.h"

#include "RenderPass.h"
#include "Stellar/Renderer/Image.h"

namespace Stellar {
	class STLR_API StandardRenderPass : public RenderPass {
	public:
		explicit StandardRenderPass(const VkFormat& swapChainImageFormat, const VkFormat& depthFormat);
		StandardRenderPass(const std::vector<VkAttachmentDescription>& attachmentDescriptions, const std::vector<VkAttachmentReference>& colorAttachmentReferences, VkAttachmentReference depthAttachmentReference);
		~StandardRenderPass() override = default;
	};
}