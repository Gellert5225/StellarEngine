#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Texture.h"
#include "Stellar/Renderer/Buffer.h"

#include <glm/glm.hpp>
#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Renderer/VulkanRenderer.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/Image/VulkanImage.h"
#include "Stellar/Platform/Vulkan/VulkanCommon.h"
#endif

namespace Stellar {
    class STLR_API Quad {
    public:
        Quad();
        ~Quad();
    private:
        glm::mat4 viewProjection;
        glm::mat4 model;
        glm::vec3 color;
        Texture2D* texture;

        // Vulkan
        #if defined(__linux__) || defined(_WIN64)
        VkDescriptorSet descriptorSet;
        #endif
    };    
}