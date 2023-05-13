#pragma once

#include "Stellar/Renderer/RenderPass.h"

namespace Stellar {
	class MetalRenderPass : public RenderPass {
	public:
		explicit MetalRenderPass(const RenderPassSpecification& spec);

		virtual ~MetalRenderPass() {}
	};
}