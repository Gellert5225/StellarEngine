#include "stlrpch.h"
#include "DescriptorSetLayout.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

namespace Stellar {

    DescriptorSetLayout::DescriptorSetLayout() {
        VkDescriptorSetLayoutBinding uboLayoutBinding{};
        uboLayoutBinding.binding = 0;
        uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount = 1;
        uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &uboLayoutBinding;

        if (vkCreateDescriptorSetLayout(VulkanDevice::GetInstance()->logicalDevice(),
                                        &layoutInfo,
                                        nullptr,
                                        &m_DescriptorSetLayouts) != VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    DescriptorSetLayout::~DescriptorSetLayout() {
        vkDestroyDescriptorSetLayout(VulkanDevice::GetInstance()->logicalDevice(),
                                     m_DescriptorSetLayouts, nullptr);
    }

    VkDescriptorSetLayout DescriptorSetLayout::getDescriptorSetLayouts() const {
        return m_DescriptorSetLayouts;
    }
}