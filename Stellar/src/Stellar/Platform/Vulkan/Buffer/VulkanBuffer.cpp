#include "stlrpch.h"

#include "VulkanBuffer.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

namespace Stellar {

    VulkanBuffer::VulkanBuffer(VkDeviceSize size,
                               VkCommandBufferUsageFlags usage,
                               VkMemoryPropertyFlags property,
                               const void *data) : Buffer(size) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(VulkanDevice::GetInstance()->logicalDevice(),
                           &bufferInfo, nullptr, &m_Buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(VulkanDevice::GetInstance()->logicalDevice(),
                                      m_Buffer, &memRequirements);
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits,
                                                   property);
        if (vkAllocateMemory(VulkanDevice::GetInstance()->logicalDevice(),
                             &allocInfo, nullptr, &m_BufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }
        vkBindBufferMemory(VulkanDevice::GetInstance()->logicalDevice(),
                           m_Buffer, m_BufferMemory, 0);

        // staging
        if (data) {
            void* mapped;
            map(&mapped);
            write(mapped, data);
            unMap();
        }
    }

    VulkanBuffer::~VulkanBuffer() {
        vkDestroyBuffer(VulkanDevice::GetInstance()->logicalDevice(), m_Buffer, nullptr);
        vkFreeMemory(VulkanDevice::GetInstance()->logicalDevice(), m_BufferMemory, nullptr);
    }

    void VulkanBuffer::copy(const Buffer &dst) {
        VkCommandBuffer cb = VulkanDevice::GetInstance()->beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;  // Optional
        copyRegion.dstOffset = 0;  // Optional
        copyRegion.size = m_Size;
        vkCmdCopyBuffer(cb, m_Buffer, (VkBuffer)dst.getBuffer(), 1, &copyRegion);

        VulkanDevice::GetInstance()->endSingleTimeCommands(cb);
    }

    void *VulkanBuffer::getBuffer() const {
        return m_Buffer;
    }

    uint32_t VulkanBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(VulkanDevice::GetInstance()->physicalDevice(),
                                            &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i))
                && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    void VulkanBuffer::map(void** data) {
        vkMapMemory(VulkanDevice::GetInstance()->logicalDevice(),
                    m_BufferMemory, 0, m_Size, 0, data);
    }

    void VulkanBuffer::unMap() {
        vkUnmapMemory(VulkanDevice::GetInstance()->logicalDevice(), m_BufferMemory);
    }

    void VulkanBuffer::write(void *dst, const void *src) {
        memcpy(dst, src, m_Size);
    }
}