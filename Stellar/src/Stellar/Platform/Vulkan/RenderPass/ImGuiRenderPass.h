#pragma once

#include "RenderPass.h"
#include "Stellar/Core/Core.h"

namespace Stellar {
	class STLR_API ImGuiRenderPass : public RenderPass {
	public:
		explicit ImGuiRenderPass(const VkFormat& swapChainImageFormat);
		~ImGuiRenderPass() override = default;
	};
}