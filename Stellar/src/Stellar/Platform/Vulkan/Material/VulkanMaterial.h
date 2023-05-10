#pragma once

#include "Stellar/Renderer/Material.h"
#include "Stellar/Renderer/ShaderUniforms.h"

#include "Stellar/Platform/Vulkan/Shader/VulkanShader.h"

namespace Stellar {
	class STLR_API VulkanMaterial : public Material {
	public:
		VulkanMaterial(const STLR_Ptr<Shader>& shader, const std::string& name = "");

		virtual void invalidate() override;

		virtual void set(const std::string& name, const STLR_Ptr<Texture2D>& texture) override;
		virtual void set(const std::string& name, const STLR_Ptr<Texture2D>& texture, uint32_t arrayIndex) override;

		virtual STLR_Ptr<Texture2D> getTexture2D(const std::string& name) override;

		virtual STLR_Ptr<Shader> getShader() override { return m_Shader; };
		virtual const std::string& getName() const override { return m_Name; };

		VkDescriptorSet getDescriptorSet(uint32_t index) const;
		void update(const std::vector<std::vector<VkWriteDescriptorSet>>& uniformBufferWriteDescriptors = std::vector<std::vector<VkWriteDescriptorSet>>());
	private:
		void init();
		void setVulkanDescriptor(const std::string& name, const STLR_Ptr<Texture2D>& texture);
		void setVulkanDescriptor(const std::string& name, const STLR_Ptr<Texture2D>& texture, uint32_t arrayIndex);
		void invalidateDescriptorSets();
		
		const ShaderResourceDeclaration* findResourceDeclaration(const std::string& name) const;
	private:
		enum class PendingDescriptorType {
			None = 0, Texture2D, TextureCube, Image2D
		};
		struct PendingDescriptor {
			PendingDescriptorType type = PendingDescriptorType::None;
			VkWriteDescriptorSet WDS;
			VkDescriptorImageInfo imageInfo{};
			STLR_Ptr<Texture2D> texture;
			STLR_Ptr<Image2D> image;
			VkDescriptorImageInfo submittedImageInfo{};
		};

		struct PendingDescriptorArray {
			PendingDescriptorType Type = PendingDescriptorType::None;
			VkWriteDescriptorSet WDS;
			std::vector<VkDescriptorImageInfo> ImageInfos;
			std::vector<STLR_Ptr<Texture2D>> Textures;
			std::vector<STLR_Ptr<Image2D>> Images;
			VkDescriptorImageInfo SubmittedImageInfo{};
		};

		std::vector<STLR_Ptr<Texture2D>> m_Textures;
		std::vector<std::vector<STLR_Ptr<Texture2D>>> m_TextureArrays;
		std::unordered_map<uint32_t, std::shared_ptr<PendingDescriptor>> m_ResidentDescriptors;
		std::unordered_map<uint32_t, std::shared_ptr<PendingDescriptorArray>> m_ResidentDescriptorArrays;
		std::vector<std::shared_ptr<PendingDescriptor>> m_PendingDescriptors;
		std::vector<std::vector<VkWriteDescriptorSet>> m_WriteDescriptors;
		VulkanShader::ShaderMaterialDescriptorSet m_DescriptorSets[3];

		std::vector<bool> m_DirtyDescriptorSets;
	};
}