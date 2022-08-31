#include "stlrpch.h"

#include "VertexBuffer.h"
#include "Stellar/Render/Vulkan/Device/VulkanDevice.h"

namespace Stellar {

    VertexBuffer::VertexBuffer(const std::vector<Vertex>* vertices)
    : vertices(vertices) {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(vertices[0]) * vertices->size();
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(VulkanDevice::GetInstance()->logicalDevice(),
                           &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create vertex buffer!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(VulkanDevice::GetInstance()->logicalDevice(),
                                      vertexBuffer, &memRequirements);
        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
                                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT|
                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        if (vkAllocateMemory(VulkanDevice::GetInstance()->logicalDevice(),
                             &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate vertex buffer memory!");
        }
        vkBindBufferMemory(VulkanDevice::GetInstance()->logicalDevice(),
                           vertexBuffer, vertexBufferMemory, 0);

        void* data;
        vkMapMemory(VulkanDevice::GetInstance()->logicalDevice(), vertexBufferMemory, 0, bufferInfo.size, 0, &data);
        memcpy(data, vertices->data(), (size_t) bufferInfo.size);
        vkUnmapMemory(VulkanDevice::GetInstance()->logicalDevice(), vertexBufferMemory);
    }

    VertexBuffer::~VertexBuffer() {
        vkDestroyBuffer(VulkanDevice::GetInstance()->logicalDevice(), vertexBuffer, nullptr);
        vkFreeMemory(VulkanDevice::GetInstance()->logicalDevice(), vertexBufferMemory, nullptr);
    }

    VkBuffer VertexBuffer::getVertexBuffer() const {
        return vertexBuffer;
    }

    uint32_t VertexBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
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

    const std::vector<Vertex>* VertexBuffer::getVertices() {
        return vertices;
    }
}