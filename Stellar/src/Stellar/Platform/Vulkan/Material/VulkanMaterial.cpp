#include "stlrpch.h"

#include "VulkanMaterial.h"

namespace Stellar {
	VulkanMaterial::VulkanMaterial(const Ref<Shader>& shader, const std::string& name) : Material(shader, name) {
		init();
	}

	void VulkanMaterial::init() {

	}
}