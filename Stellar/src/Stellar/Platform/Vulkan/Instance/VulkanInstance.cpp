#include "stlrpch.h"
#include "VulkanInstance.h"

#include "Stellar/Platform/Vulkan/VulkanCommon.h"
#include "Stellar/Log.h"

namespace Stellar {
    VulkanInstance* VulkanInstance::s_Instance = nullptr;

    void VulkanInstance::init(const std::string& appName,
                              int appVersion,
                              const std::string& engineName,
                              int engineVersion) {
        if (VulkanValidationLayer::ValidationLayerEnabled())
            validationLayerManager = new VulkanValidationLayer();

        createVkInstance(appName, appVersion, engineName, engineVersion);

        if (validationLayerManager)
            validationLayerManager->setupDebugMessenger();
    }

    VulkanInstance::~VulkanInstance() {
        delete validationLayerManager;
        vkDestroyInstance(m_VulkanInstance, nullptr);
    }

    VulkanInstance *VulkanInstance::GetInstance() {
        if (s_Instance == nullptr)
            s_Instance = new VulkanInstance();
        return s_Instance;
    }

    void VulkanInstance::createVkInstance(const std::string& appName,
                                          int appVersion,
                                          const std::string& engineName,
                                          int engineVersion) {
        STLR_CORE_INFO("Initializing Vulkan");
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = appName.c_str();
        appInfo.applicationVersion = appVersion;
        appInfo.pEngineName = engineName.c_str();
        appInfo.engineVersion = engineVersion;
        appInfo.apiVersion = VK_API_VERSION_1_3;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = GetRequiredExtensions();
        CheckIfExtensionExists(extensions.data(), extensions.size());

        createInfo.enabledLayerCount = 0;
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
#ifndef NDEBUG
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
        VulkanValidationLayer::PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
#else
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
#endif

        if (vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance) != VK_SUCCESS) {
            throw std::runtime_error("failed to create instance!");
        }

        uint32_t instanceVersion = VK_API_VERSION_1_3;
        vkEnumerateInstanceVersion(&instanceVersion);
        // 3 macros to extract version info
        uint32_t major = VK_VERSION_MAJOR(instanceVersion);
        uint32_t minor = VK_VERSION_MINOR(instanceVersion);
        uint32_t patch = VK_VERSION_PATCH(instanceVersion);

        m_InstanceVersion = std::to_string(major) +
                "." + std::to_string(minor) + "." + std::to_string(patch);

        STLR_CORE_INFO("Vulkan Version: {0}", m_InstanceVersion);
    }

    void VulkanInstance::CheckIfExtensionExists(const char** glfwExtensions,
                                uint32_t glfwExtensionCount) {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        for (uint32_t i = 0; i < glfwExtensionCount; i++) {
            bool found = false;
            for (const auto& extension : extensions) {
                STLR_CORE_TRACE("Extension Available: {0}", extension.extensionName);
                if (!strcmp(glfwExtensions[i], extension.extensionName)){
                    STLR_CORE_INFO("Extension Found: {0}", extension.extensionName);
                    found = true;
                }
            }
            if (!found)
                throw std::runtime_error("Cannot find " + std::string(glfwExtensions[i]));
        }
    }

    std::vector<const char *> VulkanInstance::GetRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifndef NDEBUG
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        for (const auto& extension : extensions) {
            STLR_CORE_INFO("Extension Required: {0}", extension);
        }
#endif

        return extensions;
    }

    VkInstance VulkanInstance::getVkInstance() {
        return m_VulkanInstance;
    }

    VulkanValidationLayer *VulkanInstance::getValidationLayerManager() const {
        return validationLayerManager;
    }
}