#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Texture.h"

#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {
    class STLR_API MetalTexture : public Texture2D {
    public:
        MetalTexture(const std::string& filePath, const TextureSpecification& spec);
		MetalTexture(const TextureSpecification& spec, const void* data);
        ~MetalTexture() override;

		uint64_t getHash() const override;
		ImTextureID getImGuiTextureID() override;
		void generateMips() override;

        // Metal
        void invalidate();
        MTL::Texture* getTexture() { return m_Texture; }
    protected:
        virtual bool loadImage(const std::string& filePath) override;
    private:
		TextureSpecification m_Specification;
        unsigned char* m_Pixels{};
        MTL::Texture* m_Texture;
    };
}