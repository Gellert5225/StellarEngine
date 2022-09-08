#include "stlrpch.h"
#include "RenderPass.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

namespace Stellar {
    RenderPass::~RenderPass() {
        vkDestroyRenderPass(VulkanDevice::GetInstance()->logicalDevice(), renderPass, nullptr);
    }

    VkRenderPass RenderPass::getVkRenderPass() const {
        return renderPass;
    }
}
