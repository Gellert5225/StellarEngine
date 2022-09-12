#include "stlrpch.h"
#include "Pipeline.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

#include "Stellar/Log.h"

namespace Stellar {
    Pipeline::~Pipeline() {
        vkDestroyPipeline(VulkanDevice::GetInstance()->logicalDevice(), pipeline, nullptr);
        vkDestroyPipelineLayout(VulkanDevice::GetInstance()->logicalDevice(),
                                pipelineLayout, nullptr);

        vkDestroyDescriptorPool(VulkanDevice::GetInstance()->logicalDevice(), m_DescriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(VulkanDevice::GetInstance()->logicalDevice(),
                                     m_DescriptorSetLayout, nullptr);
    }

    VkPipeline* Pipeline::getPipeline() {
        return &pipeline;
    }

    VkPipelineLayout* Pipeline::getPipelineLayout() {
        return &pipelineLayout;
    }
}