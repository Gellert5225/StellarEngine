#pragma once

#include "Stellar/Core.h"
#include "Stellar/Window.h"

#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API VulkanSurface {
    public:
        static VulkanSurface* GetInstance();
        ~VulkanSurface();

        void init(GLFWwindow*);
        void createSurface(GLFWwindow* window);

        [[nodiscard]] VkSurfaceKHR getSurface() const;
    private:
        static VulkanSurface* s_Instance;
        VkSurfaceKHR surface;

        VulkanSurface() = default;
    };
}
