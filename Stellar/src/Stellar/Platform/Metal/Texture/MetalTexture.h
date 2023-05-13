#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Texture.h"

#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {
    class STLR_API MetalTexture : public Texture2D {
    public:
        MetalTexture(const std::string& filePath, bool isImGui = false);
		MetalTexture(ImageFormat format, uint32_t width, uint32_t height, const void* data);
        ~MetalTexture() override;

		uint64_t getHash() const override;
		ImTextureID getImGuiTextureID() override;

        // Metal
        void invalidate();
        MTL::Texture* getTexture() { return m_Texture; }
    protected:
        virtual bool loadImage(const std::string& filePath) override;
    private:
        uint32_t m_Width{};
        uint32_t m_Height{};
        unsigned char* m_Pixels{};
        MTL::Texture* m_Texture;
    };
}