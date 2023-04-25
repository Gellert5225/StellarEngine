#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/STLRBase.h"

#include "Stellar/Renderer/Shader.h"
#include "Stellar/Renderer/Texture.h"

namespace Stellar {
	class Material : public STLR_Base {
	public:
		static STLR_Ptr<Material> Create(const STLR_Ptr<Shader>& shader, const std::string& name = "");
		virtual ~Material() {}

		virtual void invalidate() = 0;

		virtual void set(const std::string& name, const STLR_Ptr<Texture2D>& texture) = 0;

		virtual STLR_Ptr<Texture2D> getTexture2D(const std::string& name) = 0;

		virtual STLR_Ptr<Shader> getShader() = 0;
		virtual const std::string& getName() const = 0;
	protected:
		Material(const STLR_Ptr<Shader>& shader, const std::string& name) 
			: m_Shader(shader), m_Name(name) {}
		STLR_Ptr<Shader> m_Shader;
		std::vector<STLR_Ptr<Texture2D>> m_Textures;
		std::string m_Name;
	};
}