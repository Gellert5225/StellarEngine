#pragma once 

#include "Stellar/Core/Core.h"

#include "Stellar/Renderer/FrameBuffer.h"

#include <string>

namespace Stellar {
	struct RenderPassSpecification {
		Ref<FrameBuffer> targetFramebuffer;
		std::string debugName;
	};

	class RenderPass {
	public:
		~RenderPass() = default;

		RenderPassSpecification& getSpecification() { return m_Specification; };
		const RenderPassSpecification& getSpecification() const { return m_Specification; };
	
		static Ref<RenderPass> Create(const RenderPassSpecification& spec);
	protected:
		RenderPassSpecification m_Specification;
	};
}