#pragma once

#include "Stellar/Renderer/Material.h"

namespace Stellar {
	class STLR_API VulkanMaterial : public Material {
	public:
		VulkanMaterial(const STLR_Ptr<Shader>& shader, const std::string& name = "");

		virtual STLR_Ptr<Shader> getShader() override { return m_Shader; };
		virtual const std::string& getName() const override { return m_Name; };
	
	private:
		void init();
	};
}