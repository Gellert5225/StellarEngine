#pragma once

#include "Stellar/Renderer/UniformBufferSet.h"

namespace Stellar {
	class MetalUniformBufferSet : public UniformBufferSet {
	public:
		MetalUniformBufferSet(uint32_t frames): m_Frames(frames) {}
		virtual ~MetalUniformBufferSet() {}

		virtual void create(uint32_t size, uint32_t binding) override;
		virtual STLR_Ptr<UniformBuffer> get(uint32_t binding, uint32_t set = 0, uint32_t frame = 0) override;
		virtual void set(STLR_Ptr<UniformBuffer> uniformBuffer, uint32_t set = 0, uint32_t frame = 0) override;
		
	private:
		uint32_t m_Frames;

		// <frame, <set, <binding, buffer>>>
		std::map<uint32_t, std::map<uint32_t, std::map<uint32_t, STLR_Ptr<UniformBuffer>>>> m_UniformBuffers;
	};
}