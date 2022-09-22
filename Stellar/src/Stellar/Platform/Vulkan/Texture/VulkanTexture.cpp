#include "stlrpch.h"
#include "VulkanTexture.h"

#include "Stellar/Log.h"

#include "Stellar/Platform/Vulkan/Buffer/VulkanBuffer.h"
#include "Stellar/Platform/Vulkan/Image/VulkanImage.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

namespace Stellar {
    VulkanTexture::VulkanTexture(const std::string& filePath) : Texture2D(filePath) {
        bool loaded = loadImage(filePath);
        ImageSpecification imageSpec;
		imageSpec.format = ImageFormat::SRGB;
		imageSpec.width = m_Width;
		imageSpec.height = m_Height;
		imageSpec.mips = 1;
        m_Image = Image2D::Create(imageSpec);

        invalidate();
    }

    bool VulkanTexture::loadImage(const std::string& filePath) {
        int texWidth, texHeight, texChannels;
        m_Pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
        m_ImageSize = texWidth * texHeight * 4;

        STLR_CORE_ASSERT(m_Pixels, "Failed to load image {0}", filePath);

        if (!m_Pixels) return false;

        m_Width = texWidth;
        m_Height = texHeight;
        return true;
    }

    void VulkanTexture::invalidate() {
        m_Image->release();

        // staging
        auto stagingBuffer = new VulkanBuffer(m_ImageSize, 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            m_Pixels);

        stbi_image_free(m_Pixels);

        // create image
        m_Image->invalidate();

        // transition image layout
        auto info = (VulkanImageInfo*)m_Image->getImageInfo();
        auto device = VulkanDevice::GetInstance();

        VkCommandBuffer commandBuffer = device->beginSingleTimeCommands();

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
        barrier.srcAccessMask = 0; // TODO
        barrier.dstAccessMask = 0; // TODO

        vkCmdPipelineBarrier(
            commandBuffer,
            0 /* TODO */, 0 /* TODO */,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );
        
        device->endSingleTimeCommands(commandBuffer);

    }

    Image2D* VulkanTexture::getImage() const {
        return m_Image;
    }
}