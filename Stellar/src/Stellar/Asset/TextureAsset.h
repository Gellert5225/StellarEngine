#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/STLRBase.h"
#include "Stellar/Renderer/Texture.h"
#include "Asset.h"

namespace Stellar {
	class TextureAsset : public Asset {
	public:
		TextureAsset() {
			type = AssetType::Texture;
		}

		STLR_Ptr<Texture2D> getTexture() const { return m_Texture; }
		void setTexture(STLR_Ptr<Texture2D> texture) { m_Texture = texture; }

		const TextureSpecification& getSpecification() const { return m_Specification; }
		void setSpecification(const TextureSpecification& spec) { m_Specification = spec; }

	private:
		STLR_Ptr<Texture2D> m_Texture;
		TextureSpecification m_Specification;
	};
}
