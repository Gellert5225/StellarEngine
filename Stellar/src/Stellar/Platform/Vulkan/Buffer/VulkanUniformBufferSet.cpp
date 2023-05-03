#include "stlrpch.h"
#include "VulkanUniformBufferSet.h"

#include "Stellar/Platform/Vulkan/Buffer/VulkanUniformBuffer.h"

#include "VulkanBuffer.h"

namespace Stellar {
	void VulkanUniformBufferSet::create(uint32_t size, uint32_t binding) {
		for (uint32_t frame = 0; frame < m_Frames; frame++) {
			STLR_Ptr<VulkanUniformBuffer> uniformBuffer = STLR_Ptr<VulkanUniformBuffer>::Create(size, binding);
			set(uniformBuffer, 0, frame);
		}
	}

	void VulkanUniformBufferSet::set(STLR_Ptr<UniformBuffer> uniformBuffer, uint32_t set, uint32_t frame) {
		m_UniformBuffers[frame][set][uniformBuffer->getBinding()] = uniformBuffer;
	}

	STLR_Ptr<UniformBuffer> VulkanUniformBufferSet::get(uint32_t binding, uint32_t set, uint32_t frame) {
		STLR_CORE_ASSERT(m_UniformBuffers.find(frame) != m_UniformBuffers.end());
		STLR_CORE_ASSERT(m_UniformBuffers.at(frame).find(set) != m_UniformBuffers.at(frame).end());
		STLR_CORE_ASSERT(m_UniformBuffers.at(frame).at(set).find(binding) != m_UniformBuffers.at(frame).at(set).end());

		return m_UniformBuffers.at(frame).at(set).at(binding);
	}
}