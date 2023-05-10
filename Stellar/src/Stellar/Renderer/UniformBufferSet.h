#pragma once

#include "UniformBuffer.h"

namespace Stellar {
	class UniformBufferSet : public STLR_Base {
	public:
		virtual ~UniformBufferSet() {}
		virtual void create(uint32_t size, uint32_t binding) = 0;
		virtual STLR_Ptr<UniformBuffer> get(uint32_t binding, uint32_t set = 0, uint32_t frame = 0) = 0;
		virtual void set(STLR_Ptr<UniformBuffer> uniformBuffer, uint32_t set = 0, uint32_t frame = 0) = 0;

		static STLR_Ptr<UniformBufferSet> Create(uint32_t frames);
	};
}