#include "stlrpch.h"

#include "MetalMaterial.h"

namespace Stellar {
	MetalMaterial::MetalMaterial(const STLR_Ptr<Shader>& shader, const std::string& name) : Material(shader, name) {

	}

	void MetalMaterial::invalidate() {

	}

	void MetalMaterial::set(const std::string& name, const STLR_Ptr<Texture2D>& texture) {
		
	}

	void MetalMaterial::set(const std::string& name, const STLR_Ptr<Texture2D>& texture, uint32_t arrayIndex) {

	}

	STLR_Ptr<Texture2D> MetalMaterial::getTexture2D(const std::string& name) {
		return nullptr;	
	}
}