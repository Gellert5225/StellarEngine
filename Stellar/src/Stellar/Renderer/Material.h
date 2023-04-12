#pragma once

#include "Stellar/Core/Core.h"

#include "Stellar/Renderer/Shader.h"

namespace Stellar {
	class STLR_API Material {
	public:
		static Ref<Material> Create(const Ref<Shader>& shader, const std::string& name = "");
		virtual ~Material() {}

		virtual Ref<Shader> getShader() = 0;
		virtual const std::string& getName() const = 0;
	protected:
		Material(const Ref<Shader>& shader, const std::string& name) 
			: m_Shader(shader), m_Name(name) {}
		Ref<Shader> m_Shader;
		std::string m_Name;
	};
}