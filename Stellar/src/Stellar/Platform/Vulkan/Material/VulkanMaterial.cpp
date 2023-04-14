#include "stlrpch.h"

#include "VulkanMaterial.h"

namespace Stellar {
	VulkanMaterial::VulkanMaterial(const STLR_Ptr<Shader>& shader, const std::string& name) : Material(shader, name) {
		init();
	}

	void VulkanMaterial::init() {

	}
}