#include "stlrpch.h"
#include "MetalTexture.h"

#include "Stellar/Core/Log.h"

#include "Stellar/Platform/Metal/Device/MetalDevice.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Stellar {

    MetalTexture::MetalTexture(const std::string& filePath) : Texture2D(filePath) {
        bool loaded = loadImage(filePath);
        if (!loaded) {
            STLR_CORE_ERROR("Failed to load texture {0}", filePath);
            loadImage("../Resources/Textures/ErrorTexture.png");
        }

        invalidate();
    }

    MetalTexture::~MetalTexture() {
        m_Texture->release();
    }

    void MetalTexture::invalidate() {
        if (m_Texture)
            m_Texture->release();

        MTL::TextureDescriptor* textureDesc = MTL::TextureDescriptor::alloc()->init();
        textureDesc->setWidth(m_Width);
        textureDesc->setHeight(m_Height);
        textureDesc->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
        textureDesc->setTextureType(MTL::TextureType2D);
        textureDesc->setStorageMode(MTL::StorageModeManaged);
        textureDesc->setUsage(MTL::ResourceUsageSample | MTL::ResourceUsageRead);
        
        m_Texture = MetalDevice::GetInstance()->getDevice()->newTexture(textureDesc);
        m_Texture->replaceRegion(MTL::Region( 0, 0, 0, m_Width, m_Height, 1 ), 0, m_Pixels, m_Width * 4);

        textureDesc->release();
        stbi_image_free(m_Pixels);
    }

    bool MetalTexture::loadImage(const std::string& filePath) {
        int texWidth, texHeight, texChannels;
        m_Pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        if (!m_Pixels) return false;

        m_Width = texWidth;
        m_Height = texHeight;
        return true;
    }
}