#include "stlrpch.h"
#include "VulkanTexture.h"

#include "Stellar/Core/Log.h"

#include "Stellar/Platform/Vulkan/Buffer/VulkanBuffer.h"
#include "Stellar/Platform/Vulkan/Image/VulkanImage.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/Renderer/VulkanRenderer.h"
#include "Stellar/Platform/Vulkan/ImGui/imgui_impl_vulkan.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"

#include "Stellar/Platform/Vulkan/ImGui/imgui_impl_vulkan.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Stellar {
	VulkanTexture::VulkanTexture(const std::string& filePath, const TextureSpecification& spec) : Texture2D(filePath), m_Specification(spec) {
		m_Specification.generateMips = m_Specification.generateMips && !m_Specification.isImGuiTexture;
		bool loaded = loadImage(filePath);
		if (!loaded) {
			STLR_CONSOLE_LOG_ERROR("Failed to load texture {0}", filePath);
			loadImage("../Resources/Textures/ErrorTexture.png");
		}
		ImageSpecification imageSpec;
		imageSpec.format = m_Specification.format;
		imageSpec.usage = ImageUsage::Texture;
		imageSpec.width = m_Specification.width;
		imageSpec.height = m_Specification.height;
		imageSpec.mips = m_Specification.generateMips ? Texture2D::GetMipCount(m_Specification.width, m_Specification.height) : 1;
		m_Image = Image2D::Create(imageSpec);

		invalidate();

		if (spec.isImGuiTexture) {
			m_IsImGuiTexture = true;
			auto image = getImage();
			auto imageInfo = (VulkanImageInfo*)image->getImageInfo();
			m_TextureId = ImGui_ImplVulkan_AddTexture_Original(imageInfo->sampler, imageInfo->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		}
	}

	VulkanTexture::VulkanTexture(const TextureSpecification& spec, const void* data) : m_Specification(spec) {
		m_ImageSize = m_Specification.width * m_Specification.height * 4;
		void* imageData = new uint8_t[m_ImageSize];
		if (data == nullptr) {
			uint32_t whiteTex = 0xffffffff;
			data = &whiteTex;
		}
		memcpy(imageData, data, m_ImageSize);
		m_Pixels = (unsigned char*)imageData;

		ImageSpecification imageSpec;
		imageSpec.format = m_Specification.format;
		imageSpec.usage = ImageUsage::Texture;
		imageSpec.width = m_Specification.width;
		imageSpec.height = m_Specification.height;
		imageSpec.mips = m_Specification.generateMips ? Texture2D::GetMipCount(m_Specification.width, m_Specification.height) : 1;
		m_Image = Image2D::Create(imageSpec);

		invalidate();
	}

	VulkanTexture::~VulkanTexture() {
		if (m_Image)
			m_Image->release();
	}

	bool VulkanTexture::loadImage(const std::string& filePath) {
		int texWidth, texHeight, texChannels;
		m_Pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
		m_ImageSize = texWidth * texHeight * 4;

		if (!m_Pixels) return false;

		m_Specification.width = texWidth;
		m_Specification.height = texHeight;
		return true;
	}

	void VulkanTexture::invalidate() {
		m_Image->release();

		// staging
		auto stagingBuffer = new VulkanBuffer(m_ImageSize, 
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			m_Pixels, false);

		stbi_image_free(m_Pixels);

		// create image
		m_Image->invalidate();

		auto info = (VulkanImageInfo*)m_Image->getImageInfo();
		auto device = VulkanDevice::GetInstance();

		// transition image layout
		VkCommandBuffer commandBuffer1 = device->beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = info->image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		vkCmdPipelineBarrier(
			commandBuffer1,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);
		
		device->endSingleTimeCommands(commandBuffer1);
		
		// copy buffer to image
		VkCommandBuffer commandBuffer2 = device->beginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = {0, 0, 0};
		region.imageExtent = { m_Specification.width,  m_Specification.height, 1 };

		vkCmdCopyBufferToImage(
			commandBuffer2,
			(VkBuffer)stagingBuffer->getBuffer(),
			info->image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1,
			&region
		);

		device->endSingleTimeCommands(commandBuffer2);

		// // to access it in shader:
		uint32_t mipCount = m_Specification.generateMips ? Texture2D::GetMipCount(m_Specification.width, m_Specification.height) : 1;
		if (mipCount <= 1) {
			VkCommandBuffer commandBuffer3 = device->beginSingleTimeCommands();

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(
				commandBuffer3,
				VK_PIPELINE_STAGE_TRANSFER_BIT, 
				VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &barrier
			);
			
			device->endSingleTimeCommands(commandBuffer3);
		} else {
			// VkCommandBuffer commandBuffer3 = device->beginSingleTimeCommands();

			// barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			// barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			// barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			// barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			// vkCmdPipelineBarrier(
			// 	commandBuffer3,
			// 	VK_PIPELINE_STAGE_TRANSFER_BIT, 
			// 	VK_PIPELINE_STAGE_TRANSFER_BIT,
			// 	0,
			// 	0, nullptr,
			// 	0, nullptr,
			// 	1, &barrier
			// );
			
			// device->endSingleTimeCommands(commandBuffer3);
		}

		delete stagingBuffer;

		m_Image.As<VulkanImage2D>()->updateDescriptor();

		const auto mipLevels = Texture2D::GetMipCount(m_Specification.width, m_Specification.height);

		if (m_Specification.generateMips && mipLevels > 1) generateMips();
	}

	void VulkanTexture::generateMips() {
		VkFormatProperties formatProperties;
    	vkGetPhysicalDeviceFormatProperties(VulkanDevice::GetInstance()->physicalDevice(), Utils::VulkanImageFormat(m_Specification.format), &formatProperties);

		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
			throw std::runtime_error("texture image format does not support linear blitting!");
		}

		auto device = VulkanDevice::GetInstance();

		auto info = (VulkanImageInfo*)m_Image->getImageInfo();

		VkCommandBuffer commandBuffer = device->beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = info->image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		const auto mipLevels = Texture2D::GetMipCount(m_Specification.width, m_Specification.height);
		int32_t width = (int32_t)m_Specification.width;
		int32_t height = (int32_t)m_Specification.height;

        for (uint32_t i = 1; i < mipLevels; i++) {
            barrier.subresourceRange.baseMipLevel = i - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            VkImageBlit blit{};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {width, height, 1};
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = {width > 1 ? width / 2 : 1, height > 1 ? height / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer,
                info->image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                info->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            if (width > 1) width /= 2;
            if (height > 1) height /= 2;
        }

		barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

		device->endSingleTimeCommands(commandBuffer);
	}

	STLR_Ptr<Image2D> VulkanTexture::getImage() const {
		return m_Image;
	}

	uint64_t VulkanTexture::getHash() const {
		auto info = (VulkanImageInfo*)getImage()->getImageInfo();
		return (uint64_t)info->image;
	}

	ImTextureID VulkanTexture::getImGuiTextureID() {
		STLR_CORE_ASSERT(m_IsImGuiTexture, "Cannot get ImTextureID on a non-ImGui texure");
		return m_TextureId;
	}
}