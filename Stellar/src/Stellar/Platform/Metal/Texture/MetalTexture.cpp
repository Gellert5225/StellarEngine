#include "stlrpch.h"
#include "MetalTexture.h"

#include "Stellar/Core/Log.h"

#include "Stellar/Platform/Metal/Device/MetalDevice.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Stellar {

    MetalTexture::MetalTexture(const std::string& filePath, const TextureSpecification& spec) : Texture2D(filePath), m_Specification(spec) {
        bool loaded = loadImage(filePath);
        if (!loaded) {
            STLR_CORE_ERROR("Failed to load texture {0}", filePath);
            loadImage("../Resources/Textures/ErrorTexture.png");
        }

        invalidate();

		if (spec.isImGuiTexture) {
			m_IsImGuiTexture = true;
			m_TextureId = m_Texture;
		}
    }

	MetalTexture::MetalTexture(const TextureSpecification& spec, const void* data) : m_Specification(spec) {
		m_Pixels = new uint8_t[spec.width * spec.height * 4];
		if (data == nullptr) {
			for (size_t y = 0; y < spec.width * spec.height * 4; ++y) {
				m_Pixels[y] = 0xFF;
			}
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
        textureDesc->setWidth(m_Specification.width);
        textureDesc->setHeight(m_Specification.height);
        textureDesc->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
        textureDesc->setTextureType(MTL::TextureType2D);
        textureDesc->setStorageMode(MTL::StorageModeManaged);
        textureDesc->setUsage(MTL::ResourceUsageSample | MTL::ResourceUsageRead);
        
        m_Texture = MetalDevice::GetInstance()->getDevice()->newTexture(textureDesc);
        m_Texture->replaceRegion(MTL::Region( 0, 0, 0, m_Specification.width, m_Specification.height, 1 ), 0, m_Pixels, m_Specification.width * 4);

        textureDesc->release();
        stbi_image_free(m_Pixels);
    }

    bool MetalTexture::loadImage(const std::string& filePath) {
        int texWidth, texHeight, texChannels;
        m_Pixels = stbi_load(filePath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

        if (!m_Pixels) return false;

        m_Specification.width = texWidth;
        m_Specification.height = texHeight;
        return true;
    }

	uint64_t MetalTexture::getHash() const {
		void* bytes;
		m_Texture->getBytes(bytes, m_Specification.width * 4, MTL::Region( 0, 0, 0, m_Specification.width, m_Specification.height, 1 ), 0);
		return (uint64_t)bytes;
	}

	ImTextureID MetalTexture::getImGuiTextureID() {
		STLR_CORE_ASSERT(m_IsImGuiTexture, "Cannot get ImTextureID on a non-ImGui texure");

		return m_TextureId;
	}

	void MetalTexture::generateMips() {

	}
}