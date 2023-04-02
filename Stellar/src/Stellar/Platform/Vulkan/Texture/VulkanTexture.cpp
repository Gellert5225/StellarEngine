#include "stlrpch.h"
#include "VulkanTexture.h"

#include "Stellar/Core/Log.h"

#include "Stellar/Platform/Vulkan/Buffer/VulkanBuffer.h"
#include "Stellar/Platform/Vulkan/Image/VulkanImage.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/Renderer/VulkanRenderer.h"
#include "Stellar/Platform/Vulkan/ImGui/imgui_impl_vulkan.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Stellar {
    VulkanTexture::VulkanTexture(const std::string& filePath) : Texture2D(filePath) {
        bool loaded = loadImage(filePath);
        if (!loaded) {
            STLR_CORE_ERROR("Failed to load texture {0}", filePath);
            loadImage("../Resources/Textures/ErrorTexture.png");
        }
        ImageSpecification imageSpec;
		imageSpec.format = ImageFormat::RGBA;
        imageSpec.usage = ImageUsage::Texture;
		imageSpec.width = m_Width;
		imageSpec.height = m_Height;
		imageSpec.mips = 1;
        m_Image = Image2D::Create(imageSpec);

        invalidate();

        // binding
        auto device = VulkanDevice::GetInstance()->logicalDevice();
        auto pipeline = VulkanRenderer::GetPipeline();
        auto textureLayout = pipeline->getTextureSetLayout();

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pipeline->getDescriptorPool();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &textureLayout;

        VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &m_DescriptorSet));

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_DescriptorSet;
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &((VulkanImage2D*)m_Image)->getDescriptorInfo();

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
        // auto imageInfo = (VulkanImageInfo*)m_Image->getImageInfo();
        // m_ImGuiDescriptorSet = ImGui_ImplVulkan_AddTexture(imageInfo->sampler, imageInfo->imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
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
        region.imageExtent = { m_Width, m_Height, 1 };

        vkCmdCopyBufferToImage(
            commandBuffer2,
            (VkBuffer)stagingBuffer->getBuffer(),
            info->image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        device->endSingleTimeCommands(commandBuffer2);

        // to access it in shader:
        VkCommandBuffer commandBuffer3 = device->beginSingleTimeCommands();

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
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

        delete stagingBuffer;

        ((VulkanImage2D*)m_Image)->updateDescriptor();
    }

    Image2D* VulkanTexture::getImage() const {
        return m_Image;
    }
}