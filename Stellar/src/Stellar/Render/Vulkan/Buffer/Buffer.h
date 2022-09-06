#pragma once

#include "Stellar/Core.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <array>

namespace Stellar {
    struct Vertex {
        glm::vec2 position;
        glm::vec3 color;

        static VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription{};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(Vertex);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

            return bindingDescription;
        }

        static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
            std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
            attributeDescriptions[0].binding = 0;
            attributeDescriptions[0].location = 0;
            attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
            attributeDescriptions[0].offset = offsetof(Vertex, position);

            attributeDescriptions[1].binding = 0;
            attributeDescriptions[1].location = 1;
            attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
            attributeDescriptions[1].offset = offsetof(Vertex, color);

            return attributeDescriptions;
        }
    };

    class STLR_API Buffer {
    public:
        explicit Buffer(VkDeviceSize size,
                              VkCommandBufferUsageFlags usage,
                              VkMemoryPropertyFlags property,
                              const void* data = nullptr);
        ~Buffer();

        [[nodiscard]] VkBuffer getBuffer() const;
    private:
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceSize size;
        VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
        uint32_t findMemoryType(uint32_t, VkMemoryPropertyFlags);
    };
}