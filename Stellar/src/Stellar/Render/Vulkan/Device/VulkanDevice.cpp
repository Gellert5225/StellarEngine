#include "stlrpch.h"
#include "VulkanDevice.h"
#include "Stellar/Render/Vulkan/VulkanCommon.h"
#include "Stellar/Log.h"

namespace Stellar {

    VulkanDevice* VulkanDevice::s_Instance = nullptr;

    VulkanDevice* VulkanDevice::GetInstance() {
        if (s_Instance == nullptr)
            s_Instance = new VulkanDevice();
        return s_Instance;
    }

    VulkanDevice::~VulkanDevice() {
        vkDestroyDevice(m_LogicalDevice, nullptr);
    }

    void VulkanDevice::init(VkSurfaceKHR surfaceRef) {
        surface = surfaceRef;
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
        STLR_CORE_INFO("Create Logical Device");
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

        vkGetDeviceQueue(m_LogicalDevice, m_Indices.presentFamily.value(), 0, &presentQueue);
        vkGetDeviceQueue(m_LogicalDevice, m_Indices.graphicsFamily.value(), 0, &graphicsQueue);
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
            SwapChain::SwapChainSupportDetails support = querySwapChainSupport(device);
            swapChainAdequate = !support.formats.empty() && !support.presentModes.empty();
        }

        return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
                && deviceFeatures.geometryShader
                && indices.isComplete()
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
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

            if (presentSupport)
                indices.presentFamily = i;
            if (indices.isComplete())
                break;
            i++;
        }

        return indices;
    }

    SwapChain::SwapChainSupportDetails VulkanDevice::getSwapChainSupport() const {
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

    SwapChain::SwapChainSupportDetails VulkanDevice::querySwapChainSupport(VkPhysicalDevice device) const {
        SwapChain::SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
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
}