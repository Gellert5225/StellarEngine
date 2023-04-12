#pragma once

#include "Stellar/Renderer/Material.h"

namespace Stellar {
	class STLR_API VulkanMaterial : public Material {
	public:
		VulkanMaterial(const Ref<Shader>& shader, const std::string& name = "");

		virtual Ref<Shader> getShader() override { return m_Shader; };
		virtual const std::string& getName() const override { return m_Name; };
	};
}