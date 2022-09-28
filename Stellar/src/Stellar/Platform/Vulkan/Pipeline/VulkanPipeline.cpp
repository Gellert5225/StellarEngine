#include "stlrpch.h"
#include "VulkanPipeline.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

#include "Stellar/Core/Log.h"

namespace Stellar {
    VulkanPipeline::~VulkanPipeline() {
        vkDestroyPipeline(VulkanDevice::GetInstance()->logicalDevice(), m_Pipeline, nullptr);
        vkDestroyPipelineLayout(VulkanDevice::GetInstance()->logicalDevice(),
                                m_PipelineLayout, nullptr);

        vkDestroyDescriptorPool(VulkanDevice::GetInstance()->logicalDevice(), m_DescriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(VulkanDevice::GetInstance()->logicalDevice(),
                                     m_DescriptorSetLayout, nullptr);
    }

    VkPipeline* VulkanPipeline::getPipeline() {
        return &m_Pipeline;
    }

    VkPipelineLayout* VulkanPipeline::getPipelineLayout() {
        return &m_PipelineLayout;
    }
}