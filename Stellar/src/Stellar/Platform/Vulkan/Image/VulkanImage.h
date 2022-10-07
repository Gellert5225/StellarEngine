#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/Log.h"

#include "Stellar/Renderer/Image.h"

#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"

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

    namespace Utils {

		inline VkFormat VulkanImageFormat(ImageFormat format) {
			switch (format) {
                case ImageFormat::RED8UN:               return VK_FORMAT_R8_UNORM;
                case ImageFormat::RED8UI:               return VK_FORMAT_R8_UINT;
                case ImageFormat::RED16UI:              return VK_FORMAT_R16_UINT;
                case ImageFormat::RED32UI:              return VK_FORMAT_R32_UINT;
                case ImageFormat::RED32F:				return VK_FORMAT_R32_SFLOAT;
                case ImageFormat::RG8:				    return VK_FORMAT_R8G8_UNORM;
                case ImageFormat::RG16F:				return VK_FORMAT_R16G16_SFLOAT;
                case ImageFormat::RG32F:				return VK_FORMAT_R32G32_SFLOAT;
                case ImageFormat::RGBA:					return VK_FORMAT_R8G8B8A8_UNORM;
                case ImageFormat::SRGB:                 return VK_FORMAT_R8G8B8_SRGB;
                case ImageFormat::RGBA16F:				return VK_FORMAT_R16G16B16A16_SFLOAT;
                case ImageFormat::RGBA32F:				return VK_FORMAT_R32G32B32A32_SFLOAT;
                case ImageFormat::B10R11G11UF:			return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
                case ImageFormat::DEPTH32FSTENCIL8UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;
                case ImageFormat::DEPTH32F:				return VK_FORMAT_D32_SFLOAT;
                case ImageFormat::DEPTH24STENCIL8:		return VulkanDevice::GetInstance()->getDepthFormat();
                case ImageFormat::None:                 return VK_FORMAT_UNDEFINED;
			}
			STLR_CORE_ASSERT(false, "Unknown image format");
			return VK_FORMAT_UNDEFINED;
		}

	}
}