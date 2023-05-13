#pragma once

#include "Stellar/Renderer/CommandBuffer.h"

#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {
	class MetalCommandBuffer : public CommandBuffer {
	public:
		MetalCommandBuffer(uint32_t size) {};
		~MetalCommandBuffer() override {};

		virtual void begin() override {};
		virtual void end() override {};
		virtual void submit() override {};
		virtual void* getActiveCommandBuffer() override { return nullptr; };
	};
}