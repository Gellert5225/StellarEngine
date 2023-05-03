#include "stlrpch.h"
#include "VulkanUniformBufferSet.h"

#include "VulkanBuffer.h"

namespace Stellar {
	void VulkanUniformBufferSet::create(uint32_t size, uint32_t binding) {
		for (uint32_t frame = 0; frame < m_Frames; frame++) {
			STLR_Ptr<VulkanUniformBuffer> uniformBuffer = STLR_Ptr<VulkanUniformBuffer>::Create(size, binding);
			set(uniformBuffer, 0, frame);
		}
	}
}