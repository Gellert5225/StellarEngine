#pragma once 

#include "Stellar/Core/Core.h"
#include "Stellar/Core/STLRBase.h"

#include "Stellar/Renderer/FrameBuffer.h"

#include <string>

namespace Stellar {
	struct RenderPassSpecification {
		STLR_Ptr<FrameBuffer> targetFramebuffer;
		std::string debugName;
	};

	class RenderPass : public STLR_Base {
	public:
		~RenderPass() = default;

		RenderPassSpecification& getSpecification() { return m_Specification; };
		const RenderPassSpecification& getSpecification() const { return m_Specification; };
	
		static STLR_Ptr<RenderPass> Create(const RenderPassSpecification& spec);
	protected:
		RenderPassSpecification m_Specification;
	};
}