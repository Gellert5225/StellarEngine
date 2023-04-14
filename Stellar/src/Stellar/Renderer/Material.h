#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/STLRBase.h"

#include "Stellar/Renderer/Shader.h"

namespace Stellar {
	class Material : public STLR_Base {
	public:
		static STLR_Ptr<Material> Create(const STLR_Ptr<Shader>& shader, const std::string& name = "");
		virtual ~Material() {}

		virtual STLR_Ptr<Shader> getShader() = 0;
		virtual const std::string& getName() const = 0;
	protected:
		Material(const STLR_Ptr<Shader>& shader, const std::string& name) 
			: m_Shader(shader), m_Name(name) {}
		STLR_Ptr<Shader> m_Shader;
		std::string m_Name;
	};
}