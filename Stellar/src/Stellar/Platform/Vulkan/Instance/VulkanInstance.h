#pragma once

#include "Stellar/Core/Core.h"
#include "VulkanValidationLayer.h"

#include <GLFW/glfw3.h>

#include <string>

namespace Stellar {
    class VulkanValidationLayer;

    class STLR_API VulkanInstance {
    public:
        void init(const std::string& appName,
                  int appVersion,
                  const std::string& engineName,
                  int engineVersion);

        ~VulkanInstance();
        static VulkanInstance * GetInstance();
        VkInstance getVkInstance();

        [[nodiscard]] std::string getInstanceVersion() const { return m_InstanceVersion; }

        [[nodiscard]] VulkanValidationLayer* getValidationLayerManager() const;

    private:
        static VulkanInstance* s_Instance;

        std::string m_InstanceVersion;

        VkInstance m_VulkanInstance = VK_NULL_HANDLE;
        VulkanValidationLayer* validationLayerManager = nullptr;

        void createVkInstance(const std::string& appName,
                              int appVersion,
                              const std::string& engineName,
                              int engineVersion);

        static void CheckIfExtensionExists(const char** glfwExtensions,
                                           uint32_t glfwExtensionCount);
        static std::vector<const char*> GetRequiredExtensions();

        VulkanInstance() = default;
    };
}
