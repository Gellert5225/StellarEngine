#include "stlrpch.h"
#include "Pipeline.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

namespace Stellar {
    Pipeline::~Pipeline() {
        vkDestroyPipeline(VulkanDevice::GetInstance()->logicalDevice(), pipeline, nullptr);
        vkDestroyPipelineLayout(VulkanDevice::GetInstance()->logicalDevice(),
                                pipelineLayout, nullptr);
    }

    VkPipeline* Pipeline::getPipeline() {
        return &pipeline;
    }

    VkPipelineLayout* Pipeline::getPipelineLayout() {
        return &pipelineLayout;
    }
}