#include "stlrpch.h"

#include "MetalRenderPass.h"

namespace Stellar {
	MetalRenderPass::MetalRenderPass(const RenderPassSpecification& spec) {
		this->m_Specification = spec;
	}
}