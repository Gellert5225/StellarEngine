#pragma once

#include "Stellar/Core.h"
#include "Stellar/Renderer/Texture.h"

#include <vulkan/vulkan.h>

namespace Stellar {
    class STLR_API VulkanTexture : public Texture2D {
    public:
        VulkanTexture(const std::string& filePath);
        ~VulkanTexture() override;
        virtual Image2D* getImage() const override;
        virtual void bind() override;
        // Vulkan
        void invalidate();
    protected:
        virtual bool loadImage(const std::string& filePath) override;
    private:
        uint32_t m_Width;
        uint32_t m_Height;
        ImageFormat m_Format;
        unsigned char* m_Pixels;
        VkDeviceSize m_ImageSize;
    };
}