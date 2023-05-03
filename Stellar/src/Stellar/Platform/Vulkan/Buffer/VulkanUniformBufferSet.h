#pragma once

#include "Stellar/Renderer/UniformBufferSet.h"

namespace Stellar {
	class VulkanUniformBufferSet : public UniformBufferSet {
	public:
		VulkanUniformBufferSet(uint32_t frames): m_Frames(frames) {}
		virtual ~VulkanUniformBufferSet() {}

		virtual void create(uint32_t size, uint32_t binding) override;
		virtual STLR_Ptr<Buffer> get() override;
		virtual void set() override;
		
	private:
		uint32_t m_Frames;
	};
}