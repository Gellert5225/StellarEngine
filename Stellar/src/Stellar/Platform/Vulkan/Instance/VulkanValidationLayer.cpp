#include "stlrpch.h"
#include "VulkanValidationLayer.h"

#include "Stellar/Platform/Vulkan/VulkanCommon.h"
#include "VulkanInstance.h"
#include "Stellar/Log.h"

namespace Stellar {
    VulkanValidationLayer::VulkanValidationLayer() {
        #ifndef NDEBUG
            if (!CheckValidationLayerSupport())
                throw std::runtime_error("Validation layers requested, but not available!");
        #endif
    }

    VulkanValidationLayer::~VulkanValidationLayer() {
        DestroyDebugUtilsMessengerEXT(VulkanInstance::GetInstance()->getVkInstance(),
                                      debugMessenger,
                                      nullptr);
    }

    void VulkanValidationLayer::setupDebugMessenger() {
        #ifdef NDEBUG
                return;
        #endif

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        PopulateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(VulkanInstance::GetInstance()->getVkInstance(),
                                         &createInfo,
                                         nullptr,
                                         &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }

    VkResult VulkanValidationLayer::CreateDebugUtilsMessengerEXT(
            VkInstance instance,
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkDebugUtilsMessengerEXT* pDebugMessenger) {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void VulkanValidationLayer::DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                                              VkDebugUtilsMessengerEXT debugMessenger,
                                                              const VkAllocationCallbacks* pAllocator) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
            func(instance, debugMessenger, pAllocator);
    }

    void VulkanValidationLayer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                                     | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                                     | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                                 | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
    }

    bool VulkanValidationLayer::CheckValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char *layerName: validationLayers) {
            bool layerFound = false;

            for (const auto &layerProperties: availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                // TODO: log here
                return false;
            }
        }

        return true;
    }

    VkBool32 VulkanValidationLayer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                  const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                  void *pUserData) {
        if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
            STLR_CORE_TRACE("Validation layer: {0}", pCallbackData-> pMessage);
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
            STLR_CORE_INFO("Validation layer: {0}", pCallbackData->pMessage);
        else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
            STLR_CORE_WARN("Validation layer: {0}", pCallbackData->pMessage);
        else
            STLR_CORE_ERROR("Validation layer: {0}", pCallbackData->pMessage);

        return VK_FALSE;
    }

    bool VulkanValidationLayer::ValidationLayerEnabled() {
        #ifdef NDEBUG
            return false;
        #else
            return true;
        #endif
    }
}
