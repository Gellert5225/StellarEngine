#pragma once

#include "Stellar/Core/STLRBase.h"

#include "Buffer.h"

namespace Stellar {
	class UniformBuffer : public STLR_Base {
	public:
		virtual ~UniformBuffer() {}
		virtual void setData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		virtual uint32_t getBinding() const = 0;
		
		static STLR_Ptr<UniformBuffer> Create(uint32_t size, uint32_t binding);
	};
}
