#include "stlrpch.h"

#include "VulkanMaterial.h"
#include "Stellar/Platform/Vulkan/Shader/VulkanShader.h"

namespace Stellar {
	VulkanMaterial::VulkanMaterial(const STLR_Ptr<Shader>& shader, const std::string& name) : Material(shader, name) {
		init();
	}

	void VulkanMaterial::init() {

	}

	void VulkanMaterial::set(const std::string& name, const STLR_Ptr<Texture2D>& texture) {
		setVulkanDescriptor(name, texture);
	}

	void VulkanMaterial::setVulkanDescriptor(const std::string& name, const STLR_Ptr<Texture2D>& texture) {
		const ShaderResourceDeclaration* resource = findResourceDeclaration(name);
		STLR_CORE_ASSERT(resource);

		uint32_t binding = resource->getRegister();
		// Texture is already set
		if (binding < m_Textures.size() && m_Textures[binding] && texture->getHash() == m_Textures[binding]->getHash())
			return;

		if (binding >= m_Textures.size())
			m_Textures.resize(binding + 1);
		m_Textures[binding] = texture;

		const VkWriteDescriptorSet* wds = m_Shader.As<VulkanShader>()->getDescriptorSet(name);
		STLR_CORE_ASSERT(wds);
		m_ResidentDescriptors[binding] = std::make_shared<PendingDescriptor>(PendingDescriptor{ PendingDescriptorType::Texture2D, *wds, {}, texture.As<Texture2D>(), nullptr });
		m_PendingDescriptors.push_back(m_ResidentDescriptors.at(binding));

		//InvalidateDescriptorSets();
	}
}