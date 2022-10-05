#include "stlrpch.h"

#include "VulkanBuffer.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"

namespace Stellar {

    VulkanBuffer::VulkanBuffer(VkDeviceSize size,
                               VkCommandBufferUsageFlags usage,
                               VkMemoryPropertyFlags property,
                               const void *data,
                               bool useStaging) : Buffer(size) {
        auto device = VulkanDevice::GetInstance()->logicalDevice();
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_CHECK_RESULT(vkCreateBuffer(device, &bufferInfo, nullptr, &m_Buffer));

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, m_Buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, property);

        VK_CHECK_RESULT(vkAllocateMemory(device, &allocInfo, nullptr, &m_BufferMemory));

        vkBindBufferMemory(device, m_Buffer, m_BufferMemory, 0);

        // staging
        if (data) {
            if (useStaging) {
                VkDeviceMemory stagingBufferMemory;
                VkBuffer stagingBuffer;
                VkMemoryPropertyFlags stagingProperty = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

                VkBufferCreateInfo bufferInfo{};
                bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferInfo.size = size;
                bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

                VK_CHECK_RESULT(vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer));

                VkMemoryRequirements memRequirements;
                vkGetBufferMemoryRequirements(device, stagingBuffer, &memRequirements);

                VkMemoryAllocateInfo allocInfo{};
                allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                allocInfo.allocationSize = memRequirements.size;
                allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, stagingProperty);
                VK_CHECK_RESULT(vkAllocateMemory(device, &allocInfo, nullptr, &stagingBufferMemory));

                vkBindBufferMemory(device, stagingBuffer, stagingBufferMemory, 0);

                void* mapped;
                vkMapMemory(device, stagingBufferMemory, 0, m_Size, 0, &mapped);
                write(mapped, data);
                vkUnmapMemory(device, stagingBufferMemory);
                
                VkCommandBuffer cb = VulkanDevice::GetInstance()->beginSingleTimeCommands();

                VkBufferCopy copyRegion{};
                copyRegion.srcOffset = 0;  // Optional
                copyRegion.dstOffset = 0;  // Optional
                copyRegion.size = m_Size;
                vkCmdCopyBuffer(cb, stagingBuffer, m_Buffer, 1, &copyRegion);

                VulkanDevice::GetInstance()->endSingleTimeCommands(cb);

                vkDestroyBuffer(device, stagingBuffer, nullptr);
                vkFreeMemory(device, stagingBufferMemory, nullptr);
            } else {
                void* mapped;
                map(&mapped);
                write(mapped, data);
                unMap();
            }
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