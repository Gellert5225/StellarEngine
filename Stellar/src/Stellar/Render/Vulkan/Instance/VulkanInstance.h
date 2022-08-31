#pragma once

#include "Stellar/Core.h"
#include "VulkanValidationLayer.h"

#include <GLFW/glfw3.h>

namespace Stellar {
    class VulkanValidationLayer;

    class STLR_API VulkanInstance {
    public:
        void init(const std::string&,
                  int,
                  const std::string&,
                  int);

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

        void createVkInstance(const std::string&,
                              int,
                              const std::string&,
                              int);

        static void CheckIfExtensionExists(const char**, uint32_t);
        static std::vector<const char*> GetRequiredExtensions();

        VulkanInstance() = default;
    };
}
