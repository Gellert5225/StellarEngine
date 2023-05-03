#pragma once

#include "Stellar/Renderer/UniformBufferSet.h"

namespace Stellar {
	class VulkanUniformBufferSet : public UniformBufferSet {
	public:
		VulkanUniformBufferSet(uint32_t frames): m_Frames(frames) {}
		virtual ~VulkanUniformBufferSet() {}

		virtual void create(uint32_t size, uint32_t binding) override;
		virtual STLR_Ptr<UniformBuffer> get(uint32_t binding, uint32_t set = 0, uint32_t frame = 0) override;
		virtual void set(STLR_Ptr<UniformBuffer> uniformBuffer, uint32_t set = 0, uint32_t frame = 0) override;
		
	private:
		uint32_t m_Frames;
	};
}