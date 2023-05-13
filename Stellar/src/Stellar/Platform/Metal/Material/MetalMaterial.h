#pragma once

#include "Stellar/Renderer/Material.h"
#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {
	class MetalMaterial : public Material {
	public:
		MetalMaterial(const STLR_Ptr<Shader>& shader, const std::string& name = "");

		virtual void invalidate() override;

		virtual void set(const std::string& name, const STLR_Ptr<Texture2D>& texture) override;
		virtual void set(const std::string& name, const STLR_Ptr<Texture2D>& texture, uint32_t arrayIndex) override;

		virtual STLR_Ptr<Texture2D> getTexture2D(const std::string& name) override;

		virtual STLR_Ptr<Shader> getShader() override { return m_Shader; };
		virtual const std::string& getName() const override { return m_Name; };

		std::vector<STLR_Ptr<Texture2D>> getTextures() { return m_TextureArrays; }
		MTL::Texture** getMetalTextures();
	private:
		void setMetalTexture(const std::string& name, const STLR_Ptr<Texture2D>& texture);
		void setMetalTexture(const std::string& name, const STLR_Ptr<Texture2D>& texture, uint32_t arrayIndex);
	private:
		std::vector<STLR_Ptr<Texture2D>> m_TextureArrays;
		std::vector<MTL::Texture*> m_MetalTextureArrays;
	};
}