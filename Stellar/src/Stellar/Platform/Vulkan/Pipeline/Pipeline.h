#pragma once

#include "Stellar/Core.h"

#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API Pipeline {
    public:
        Pipeline() = default;
        virtual ~Pipeline() = 0;

        VkPipeline* getPipeline();
        VkPipelineLayout* getPipelineLayout();
        VkDescriptorSetLayout getDescriptorSetLayout() { return m_DescriptorSetLayout; }
        VkDescriptorPool getDescriptorPool() { return m_DescriptorPool; }

    protected:
        VkPipelineLayout m_PipelineLayout;
        VkPipeline m_Pipeline;

        VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
        VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

        virtual void createDescriptorSetLayout() = 0;
        virtual void createDescriptorPool() = 0;
    };
}