#include "stlrpch.h"
#include "Quad.h"

namespace Stellar {
    Quad::Quad() {
        auto uniformBuffer = Buffer::Create(BufferType::Uniform, sizeof(GlobalUniforms));

        // Vulkan
        #if defined(__linux__) || defined(_WIN64)
        auto device = VulkanDevice::GetInstance()->logicalDevice();
        auto pipeline = VulkanRenderer::GetPipeline();
        auto textureLayout = pipeline->getTextureSetLayout();

        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = pipeline->getDescriptorPool();
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &textureLayout;

        VK_CHECK_RESULT(vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet));

        VkWriteDescriptorSet descriptorWrite;
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSet;
        descriptorWrite.dstBinding = 1;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pImageInfo = &((VulkanImage2D*)texture->getImage())->getDescriptorInfo();

        vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
        #endif

        // void* data;
        // uniformBuffer->map(&data);
        // uniformBuffer->write(data, &ubo);
        // uniformBuffer->unMap();
    }

    Quad::~Quad() {

    }
}