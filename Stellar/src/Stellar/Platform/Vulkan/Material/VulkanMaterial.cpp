#include "stlrpch.h"

#include "VulkanMaterial.h"

#include "Stellar/Core/Application.h"
#include "Stellar/Platform/Vulkan/Shader/VulkanShader.h"
#include "Stellar/Platform/Vulkan/Device/VulkanDevice.h"
#include "Stellar/Platform/Vulkan/Image/VulkanImage.h"
#include "Stellar/Platform/Vulkan/Texture/VulkanTexture.h"

namespace Stellar {
	VulkanMaterial::VulkanMaterial(const STLR_Ptr<Shader>& shader, const std::string& name) : Material(shader, name), m_WriteDescriptors(Renderer::MAX_FRAMES_IN_FLIGHT) {
		init();
	}

	void VulkanMaterial::init() {
		invalidate();
	}

	void VulkanMaterial::invalidate() {
		uint32_t framesInFlight = Renderer::MAX_FRAMES_IN_FLIGHT;
		auto shader = m_Shader.As<VulkanShader>();
		if (shader->hasDescriptorSet(0)) {
			const auto& shaderDescriptorSets = shader->getShaderDescriptorSets();
			if (!shaderDescriptorSets.empty()) {
				for (auto&& [binding, descriptor] : m_ResidentDescriptors)
					m_PendingDescriptors.push_back(descriptor);
			}
		}
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

		//invalidateDescriptorSets();
	}

	void VulkanMaterial::invalidateDescriptorSets() {
		// uint32_t framesInFlight = Renderer::GetConfig().FramesInFlight;
		// for (int i = 0; i < framesInFlight; i++)
		// 	m_DirtyDescriptorSets[i] = true;
	}

	const ShaderResourceDeclaration* VulkanMaterial::findResourceDeclaration(const std::string& name) const {
		auto& resources = m_Shader->getResources();
		for (const auto& [n, resource] : resources) {
			if (resource.getName() == name)
				return &resource;
		}
		return nullptr;
	}

	void VulkanMaterial::update(const std::vector<std::vector<VkWriteDescriptorSet>>& uniformBufferWriteDescriptors) {
		auto device = VulkanDevice::GetInstance()->logicalDevice();
		for (auto&& [binding, descriptor] : m_ResidentDescriptors) {
			if (descriptor->type == PendingDescriptorType::Image2D) {
				STLR_Ptr<VulkanImage2D> image = descriptor->image.As<VulkanImage2D>();
				VulkanImageInfo* info = (VulkanImageInfo*)image->getImageInfo();
				STLR_CORE_ASSERT(info->imageView, "ImageView is null");
				if (descriptor->WDS.pImageInfo && info->imageView != descriptor->WDS.pImageInfo->imageView) {
					STLR_CORE_WARN("Found out of date Image2D descriptor ({0} vs. {1})",(void*)info->imageView, (void*)descriptor->WDS.pImageInfo->imageView);
					m_PendingDescriptors.emplace_back(descriptor);
					invalidateDescriptorSets();
				}
			}
		}

		uint32_t frameIndex = Renderer::GetCurrentFrameIndex();

		m_WriteDescriptors[frameIndex].clear();

		if (!uniformBufferWriteDescriptors.empty()) {
			for (auto& wd : uniformBufferWriteDescriptors[frameIndex])
				m_WriteDescriptors[frameIndex].push_back(wd);
		}

		for (auto&& [binding, pd] : m_ResidentDescriptors) {
			if (pd->type == PendingDescriptorType::Texture2D) {
				STLR_Ptr<VulkanTexture> texture = pd->texture.As<VulkanTexture>();
				STLR_Ptr<VulkanImage2D> img = texture->getImage().As<VulkanImage2D>();
				pd->imageInfo = img->getDescriptorInfo();
				pd->WDS.pImageInfo = &pd->imageInfo;
			} else {
				STLR_CORE_ASSERT(false, "Unknown descriptor type");
			}
			// else if (pd->Type == PendingDescriptorType::TextureCube) {
			// 	Ref<VulkanTextureCube> texture = pd->Texture.As<VulkanTextureCube>();
			// 	pd->ImageInfo = texture->GetVulkanDescriptorInfo();
			// 	pd->WDS.pImageInfo = &pd->ImageInfo;
			// } else if (pd->Type == PendingDescriptorType::Image2D) {
			// 	Ref<VulkanImage2D> image = pd->Image.As<VulkanImage2D>();
			// 	pd->ImageInfo = image->GetDescriptor();
			// 	pd->WDS.pImageInfo = &pd->ImageInfo;
			// }

			m_WriteDescriptors[frameIndex].push_back(pd->WDS);
		}

		auto vulkanShader = m_Shader.As<VulkanShader>();
		auto descriptorSet = vulkanShader->allocateDescriptorSet();
		m_DescriptorSets[frameIndex] = descriptorSet;
		for (auto& writeDescriptor : m_WriteDescriptors[frameIndex])
			writeDescriptor.dstSet = descriptorSet.DescriptorSets[0];

		vkUpdateDescriptorSets(device, m_WriteDescriptors[frameIndex].size(), m_WriteDescriptors[frameIndex].data(), 0, nullptr);
		m_PendingDescriptors.clear();
	}
}