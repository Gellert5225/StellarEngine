#pragma once

#include "Stellar/Renderer/UniformBuffer.h"

#include <vulkan/vulkan.h>

namespace Stellar {
	class VulkanUniformBuffer : public UniformBuffer {
	public:
		VulkanUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~VulkanUniformBuffer();

		virtual void setData(const void* data, uint32_t size, uint32_t offset = 0) override;
		virtual uint32_t getBinding() const override { return m_Binding; }

		const VkDescriptorBufferInfo& getDescriptorBufferInfo() const { return m_DescriptorInfo; }
	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
		VkDescriptorBufferInfo m_DescriptorInfo{};
		uint32_t m_Size = 0;
		uint32_t m_Binding = 0;
		std::string m_Name;
		VkShaderStageFlagBits m_ShaderStage = VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
	};
}