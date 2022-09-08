#include "stlrpch.h"

#include "Buffer.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

namespace Stellar {

    Buffer::Buffer(VkDeviceSize size,
                   VkCommandBufferUsageFlags usage,
                   VkMemoryPropertyFlags property,
                   const void* data)
    : size(size) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(VulkanDevice::GetInstance()->logicalDevice(),
                           &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(VulkanDevice::GetInstance()->logicalDevice(),
                                      buffer, &memRequirements);
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                                   property);
        if (vkAllocateMemory(VulkanDevice::GetInstance()->logicalDevice(),
                             &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }
        vkBindBufferMemory(VulkanDevice::GetInstance()->logicalDevice(),
                           buffer, bufferMemory, 0);

        if (data) {
            void* mapped;
            vkMapMemory(VulkanDevice::GetInstance()->logicalDevice(),
                        bufferMemory, 0, bufferInfo.size, 0, &mapped);
            memcpy(mapped, data, (size_t) bufferInfo.size);
            vkUnmapMemory(VulkanDevice::GetInstance()->logicalDevice(), bufferMemory);
        }
    }

    Buffer::~Buffer() {
        vkDestroyBuffer(VulkanDevice::GetInstance()->logicalDevice(), buffer, nullptr);
        vkFreeMemory(VulkanDevice::GetInstance()->logicalDevice(), bufferMemory, nullptr);
    }

    VkBuffer Buffer::getBuffer() const {
        return buffer;
    }

    uint32_t Buffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
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

    void Buffer::CopyBuffer(const Buffer &src, const Buffer &dst, uint64_t size) {
        VkCommandBuffer cb = VulkanDevice::GetInstance()->beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;  // Optional
        copyRegion.dstOffset = 0;  // Optional
        copyRegion.size = size;
        vkCmdCopyBuffer(cb, src.getBuffer(),
                        dst.getBuffer(), 1, &copyRegion);

        VulkanDevice::GetInstance()->endSingleTimeCommands(cb);
    }
}