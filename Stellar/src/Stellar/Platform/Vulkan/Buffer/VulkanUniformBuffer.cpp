#include "stlrpch.h"
#include "VulkanUniformBuffer.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanBuffer.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"

namespace Stellar {
	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size, uint32_t binding) : m_Size(size), m_Binding(binding) {
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK_RESULT(vkCreateBuffer(device, &bufferInfo, nullptr, &m_Buffer));

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, m_Buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VulkanBuffer::FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		VK_CHECK_RESULT(vkAllocateMemory(device, &allocInfo, nullptr, &m_BufferMemory));

		vkBindBufferMemory(device, m_Buffer, m_BufferMemory, 0);

		m_DescriptorInfo.buffer = m_Buffer;
		m_DescriptorInfo.offset = 0;
		m_DescriptorInfo.range = m_Size;
	}

	VulkanUniformBuffer::~VulkanUniformBuffer() {
		vkDestroyBuffer(VulkanDevice::GetInstance()->logicalDevice(), m_Buffer, nullptr);
		vkFreeMemory(VulkanDevice::GetInstance()->logicalDevice(), m_BufferMemory, nullptr);
	}

	void VulkanUniformBuffer::setData(const void* data, uint32_t size, uint32_t offset) {
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		void* local;
		vkMapMemory(device, m_BufferMemory, 0, m_Size, 0, &local);
		memcpy(local, data, m_Size);
		vkUnmapMemory(device, m_BufferMemory);
	}
}