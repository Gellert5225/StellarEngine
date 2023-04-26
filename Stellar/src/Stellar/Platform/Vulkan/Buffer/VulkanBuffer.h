#pragma once

#include "Stellar/Core/Core.h"

#include "Stellar/Renderer/Buffer.h"

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <array>

namespace Stellar {
	struct VulkanVertex : public Vertex {

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(VulkanVertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(VulkanVertex, position);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, tilingFactor);

			return attributeDescriptions;
		}
	};

	class VulkanBuffer : public Buffer {
	public:
		explicit VulkanBuffer(VkDeviceSize size,
							VkCommandBufferUsageFlags usage,
							VkMemoryPropertyFlags property,
							const void* data = nullptr,
							bool useStaging = true);
		~VulkanBuffer() override;

		static uint32_t FindMemoryType(uint32_t, VkMemoryPropertyFlags);

		void copy(const Buffer& dst) override;
		void map(void** data) override;
		void unMap() override;
		void write(void* dst, const void* src) override;

		[[nodiscard]] void* getBuffer() const override;
	private:
		VkBuffer m_Buffer = VK_NULL_HANDLE;
		VkDeviceMemory m_BufferMemory = VK_NULL_HANDLE;
	};

	class VulkanUniformBuffer : public Buffer {
		
	};
}