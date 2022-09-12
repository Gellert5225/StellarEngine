#pragma once

#include "Stellar/Core.h"

#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API DescriptorSetLayout {
    public:
        DescriptorSetLayout();
        ~DescriptorSetLayout();

        [[nodiscard]] VkDescriptorSetLayout getDescriptorSetLayouts() const;

    private:
        VkDescriptorSetLayout m_DescriptorSetLayouts;
    };
}