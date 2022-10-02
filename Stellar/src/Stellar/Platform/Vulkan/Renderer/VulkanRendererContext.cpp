#include "stlrpch.h"

#include "VulkanRendererContext.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/SwapChain/VulkanSurface.h"

#include "Stellar/Core/Application.h"
#include "Stellar/Core/Log.h"

namespace Stellar {

    VulkanRendererContext::VulkanRendererContext() {
        VulkanInstance::GetInstance()->init("Stellar Engine Sandbox", 1, "Stellar", 1);
        VulkanSurface::GetInstance()->init(Application::Get().getWindow().getGLFWWindow());
        VulkanDevice::GetInstance()->init(VulkanSurface::GetInstance()->getSurface());
    }

    VulkanRendererContext::~VulkanRendererContext() {
        delete VulkanDevice::GetInstance();
        delete VulkanSurface::GetInstance();
        delete VulkanInstance::GetInstance();
    }

    std::string VulkanRendererContext::getGPUInfo() const {
        return VulkanDevice::GetInstance()->getDeviceProperties().deviceName;
    };

    std::string VulkanRendererContext::getGraphicsAPI() const {
        return VulkanInstance::GetInstance()->getInstanceVersion();
    };
}