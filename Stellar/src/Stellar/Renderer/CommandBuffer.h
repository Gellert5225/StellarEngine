#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/STLRBase.h"

namespace Stellar {
	class CommandBuffer : public STLR_Base {
	public:
		virtual ~CommandBuffer() = default;

		virtual void begin() = 0;
		virtual void end() = 0;
		virtual void submit() = 0;
		virtual void* getActiveCommandBuffer() = 0;

		static STLR_Ptr<CommandBuffer> Create(uint32_t size);

	protected:
		void* m_ActiveCommandBuffer = nullptr;
	};
}