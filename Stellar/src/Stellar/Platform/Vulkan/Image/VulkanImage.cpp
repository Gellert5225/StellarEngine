#include "stlrpch.h"
#include "VulkanImage.h"

#include "Stellar/Log.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanBuffer.h"

namespace Stellar {
    VulkanImage2D::VulkanImage2D(const ImageSpecification& spec) : m_Specification(spec) {

    }

    VulkanImage2D::~VulkanImage2D() {
        auto device = VulkanDevice::GetInstance()->logicalDevice();
        vkDestroyImageView(device, m_Info.imageView, nullptr);
        vkDestroySampler(device, m_Info.sampler, nullptr);
        vkDestroyImage(device, m_Info.image, nullptr);
        vkFreeMemory(device, m_Info.imageMemory, nullptr);
    }

    void VulkanImage2D::invalidate() {
        release();

        auto device = VulkanDevice::GetInstance()->logicalDevice();
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = static_cast<uint32_t>(m_Specification.width);
        imageInfo.extent.height = static_cast<uint32_t>(m_Specification.height);
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.flags = 0; // Optional
        if (vkCreateImage(device, &imageInfo, nullptr, &m_Info.image) != VK_SUCCESS) {
            STLR_CORE_ASSERT(false, "Fail to create image");
        }

        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, m_Info.image, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = VulkanBuffer::FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        if (vkAllocateMemory(device, &allocInfo, nullptr, &m_Info.imageMemory) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate image memory!");
        }

        vkBindImageMemory(device, m_Info.image, m_Info.imageMemory, 0);
    }

    void VulkanImage2D::release() {
		if (m_Info.image == nullptr)
			return;

        auto device = VulkanDevice::GetInstance()->logicalDevice();
        vkDestroyImageView(device, m_Info.imageView, nullptr);
        vkDestroySampler(device, m_Info.sampler, nullptr);
        vkDestroyImage(device, m_Info.image, nullptr);
        vkFreeMemory(device, m_Info.imageMemory, nullptr);
		m_Info.image = nullptr;
		m_Info.imageView = nullptr;
		m_Info.sampler = nullptr;
    }

    ImageSpecification& VulkanImage2D::GetSpecification() {
        return m_Specification;
    }
}