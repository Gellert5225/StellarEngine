#pragma once

#include "Stellar/Core/Core.h"

#include "Stellar/Renderer/Image.h"

#include <vulkan/vulkan.h>

namespace Stellar {

	struct VulkanImageInfo {
		VkImage image = VK_NULL_HANDLE;
		VkImageView imageView = VK_NULL_HANDLE;
		VkSampler sampler = VK_NULL_HANDLE;
        VkDeviceMemory imageMemory = VK_NULL_HANDLE;
	};

    class STLR_API VulkanImage2D : public Image2D {
    public:
        explicit VulkanImage2D(const ImageSpecification& spec);
        virtual ~VulkanImage2D() override;

        virtual void invalidate() override;
        virtual void release() override;
        virtual ImageSpecification& GetSpecification() override;

        // vulkan
        void* getImageInfo() override { return (VulkanImageInfo*)&m_Info; }
        void updateDescriptor();
        [[nodiscard]] const VkDescriptorImageInfo& getDescriptorInfo() const { return m_DescriptorImageInfo; }
    private:
        ImageSpecification m_Specification{};
        VulkanImageInfo m_Info{};
        VkDescriptorImageInfo m_DescriptorImageInfo{};
    };
}