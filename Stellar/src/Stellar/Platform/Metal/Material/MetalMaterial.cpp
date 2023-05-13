#include "stlrpch.h"

#include "MetalMaterial.h"
#include "Stellar/Platform/Metal/Texture/MetalTexture.h"

namespace Stellar {
	MetalMaterial::MetalMaterial(const STLR_Ptr<Shader>& shader, const std::string& name) : Material(shader, name) {

	}

	void MetalMaterial::invalidate() {

	}

	void MetalMaterial::set(const std::string& name, const STLR_Ptr<Texture2D>& texture) {
		setMetalTexture(name, texture);
	}

	void MetalMaterial::set(const std::string& name, const STLR_Ptr<Texture2D>& texture, uint32_t arrayIndex) {
		setMetalTexture(name, texture, arrayIndex);
	}

	STLR_Ptr<Texture2D> MetalMaterial::getTexture2D(const std::string& name) {
		return nullptr;	
	}

	void MetalMaterial::setMetalTexture(const std::string& name, const STLR_Ptr<Texture2D>& texture) {

	}

	void MetalMaterial::setMetalTexture(const std::string& name, const STLR_Ptr<Texture2D>& texture, uint32_t arrayIndex) {
		if (arrayIndex >= m_TextureArrays.size()) {
			m_TextureArrays.resize(arrayIndex + 1);
			m_MetalTextureArrays.resize(arrayIndex + 1);
		}
		m_TextureArrays[arrayIndex] = texture;
		m_MetalTextureArrays[arrayIndex] = texture.As<MetalTexture>()->getTexture();
	}

	MTL::Texture** MetalMaterial::getMetalTextures() {
		return m_MetalTextureArrays.data();
	}
}