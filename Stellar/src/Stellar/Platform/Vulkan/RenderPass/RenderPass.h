#pragma once
#include "Stellar/Core/Core.h"

#include <vulkan/vulkan.h>

namespace Stellar {
	class STLR_API RenderPass {
	public:
		RenderPass() = default;
		virtual ~RenderPass() = 0;

		[[nodiscard]] VkRenderPass getVkRenderPass() const;
	protected:
		VkRenderPass renderPass;
	};
}