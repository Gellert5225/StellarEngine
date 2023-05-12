#include "stlrpch.h"
#include "VulkanImage.h"

#include "Stellar/Core/Log.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/Buffer/VulkanBuffer.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"

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

		VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT;

		if (m_Specification.usage == ImageUsage::Attachment) {
			if (Utils::IsDepthFormat(m_Specification.format))
				usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			else
				usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		} else if (m_Specification.usage == ImageUsage::Texture) {
			usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		VkImageAspectFlags aspectMask = Utils::IsDepthFormat(m_Specification.format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
		if (m_Specification.format == ImageFormat::DEPTH24STENCIL8)
			aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = static_cast<uint32_t>(m_Specification.width);
		imageInfo.extent.height = static_cast<uint32_t>(m_Specification.height);
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = m_Specification.mips;
		imageInfo.arrayLayers = m_Specification.layers;
		imageInfo.format = Utils::VulkanImageFormat(m_Specification.format);
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.flags = 0; // Optional
		VK_CHECK_RESULT(vkCreateImage(device, &imageInfo, nullptr, &m_Info.image));

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, m_Info.image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = VulkanBuffer::FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		
		VK_CHECK_RESULT(vkAllocateMemory(device, &allocInfo, nullptr, &m_Info.imageMemory));

		vkBindImageMemory(device, m_Info.image, m_Info.imageMemory, 0);

		// default image view
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = Utils::VulkanImageFormat(m_Specification.format);
		imageViewCreateInfo.flags = 0;
		imageViewCreateInfo.subresourceRange.aspectMask = aspectMask;
		imageViewCreateInfo.image = m_Info.image;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = m_Specification.mips;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = m_Specification.layers;
		VK_CHECK_RESULT(vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_Info.imageView));

		// create texture sampler
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(VulkanDevice::GetInstance()->physicalDevice(), &properties);

		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;

		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;

		VK_CHECK_RESULT(vkCreateSampler(device, &samplerInfo, nullptr, &m_Info.sampler));
	}

	void VulkanImage2D::release() {
		if (m_Info.image == nullptr)
			return;

		auto device = VulkanDevice::GetInstance()->logicalDevice();
		vkDeviceWaitIdle(device);
		vkDestroyImageView(device, m_Info.imageView, nullptr);
		vkDestroySampler(device, m_Info.sampler, nullptr);
		vkDestroyImage(device, m_Info.image, nullptr);
		//vkFreeMemory(device, m_Info.imageMemory, nullptr);
		m_Info.image = nullptr;
		m_Info.imageView = nullptr;
		m_Info.sampler = nullptr;
	}

	void VulkanImage2D::updateDescriptor() {
		m_DescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		m_DescriptorImageInfo.imageView = m_Info.imageView;
		m_DescriptorImageInfo.sampler = m_Info.sampler;
	}

	ImageSpecification& VulkanImage2D::GetSpecification() {
		return m_Specification;
	}
}