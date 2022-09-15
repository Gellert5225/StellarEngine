#include "stlrpch.h"
#include "VulkanDevice.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"
#include "Stellar/Log.h"

namespace Stellar {

    VulkanDevice* VulkanDevice::s_Instance = nullptr;

    VulkanDevice* VulkanDevice::GetInstance() {
        if (s_Instance == nullptr)
            s_Instance = new VulkanDevice();
        return s_Instance;
    }

    VulkanDevice::~VulkanDevice() {
        vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);
        vkDestroyDevice(m_LogicalDevice, nullptr);
    }

    void VulkanDevice::init(VkSurfaceKHR surfaceRef) {
        m_Surface = surfaceRef;
        pickPhysicalDevice();
        createLogicalDevice();
        createCommandPool();
    }

    void VulkanDevice::pickPhysicalDevice() {
        VkInstance instance = VulkanInstance::GetInstance()->getVkInstance();
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0)
            throw std::runtime_error("Failed to find GPUs with Vulkan support");

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                m_PhysicalDevice = device;
                break;
            }
        }

        if (m_PhysicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    void VulkanDevice::createLogicalDevice() {
        m_Indices = findQueueFamilies(m_PhysicalDevice);

        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {
                m_Indices.graphicsFamily.value(),
                m_Indices.presentFamily.value()
        };

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures deviceFeatures{};
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pEnabledFeatures = &deviceFeatures;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();
        
        VulkanValidationLayer* validationLayer = VulkanInstance::GetInstance()->getValidationLayerManager();
        if (validationLayer && VulkanValidationLayer::ValidationLayerEnabled()) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS) {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(m_LogicalDevice, m_Indices.presentFamily.value(), 0, &m_PresentQueue);
        vkGetDeviceQueue(m_LogicalDevice, m_Indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
    }

    void VulkanDevice::createCommandPool() {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = findQueueFamilies(m_PhysicalDevice).graphicsFamily.value();

        if (vkCreateCommandPool(VulkanDevice::GetInstance()->logicalDevice(), &poolInfo,
                                nullptr, &m_CommandPool) != VK_SUCCESS)
            throw std::runtime_error("Failed to create command pool");
    }

    bool VulkanDevice::isDeviceSuitable(VkPhysicalDevice device) const {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        STLR_CORE_INFO("GPU: {0}", deviceProperties.deviceName);

        Queue::QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionSupported = checkDeviceExtensionSupport(device);
        bool swapChainAdequate = false;

        if (extensionSupported) {
            VulkanSwapChain::SwapChainSupportDetails support = querySwapChainSupport(device);
            swapChainAdequate = !support.formats.empty() && !support.presentModes.empty();
        }

        return deviceProperties.deviceType == indices.isComplete()
                && extensionSupported
                && swapChainAdequate;
    }

    bool VulkanDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) const {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    Queue::QueueFamilyIndices VulkanDevice::findQueueFamilies(VkPhysicalDevice device) const {
        Queue::QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto &queueFamily: queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
                indices.graphicsFamily = i;

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

            if (presentSupport)
                indices.presentFamily = i;
            if (indices.isComplete())
                break;
            i++;
        }

        return indices;
    }

    VulkanSwapChain::SwapChainSupportDetails VulkanDevice::getSwapChainSupport() const {
        return querySwapChainSupport(m_PhysicalDevice);
    }

    VkPhysicalDevice VulkanDevice::physicalDevice() const {
        return m_PhysicalDevice;
    }

    Queue::QueueFamilyIndices VulkanDevice::getIndices() const {
        return m_Indices;
    }

    VkDevice VulkanDevice::logicalDevice() const {
        return m_LogicalDevice;
    }

    VulkanSwapChain::SwapChainSupportDetails VulkanDevice::querySwapChainSupport(VkPhysicalDevice device) const {
        VulkanSwapChain::SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkCommandPool VulkanDevice::getCommandPool() const {
        return m_CommandPool;
    }

    VkPhysicalDeviceProperties VulkanDevice::getDeviceProperties() const {
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(m_PhysicalDevice, &deviceProperties);
        return deviceProperties;
    }

    VkCommandBuffer VulkanDevice::beginSingleTimeCommands() {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_CommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        VK_CHECK_RESULT(vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, &commandBuffer));

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        //beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK_RESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo));
        return commandBuffer;
    }

    void VulkanDevice::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
        VK_CHECK_RESULT(vkEndCommandBuffer(commandBuffer));

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

//        VkFenceCreateInfo fenceCreateInfo = {};
//        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//        fenceCreateInfo.flags = 0;
//        VkFence fence;
//        VK_CHECK_RESULT(vkCreateFence(m_LogicalDevice, &fenceCreateInfo, nullptr, &fence));
        VK_CHECK_RESULT(vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE));
        VK_CHECK_RESULT(vkQueueWaitIdle(m_GraphicsQueue));

//        VK_CHECK_RESULT(vkWaitForFences(m_LogicalDevice, 1, &fence, VK_TRUE, UINT64_MAX));
//        vkDestroyFence(m_LogicalDevice, fence, nullptr);

        vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &commandBuffer);
    }
}