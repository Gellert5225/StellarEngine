#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Texture.h"

#include <vulkan/vulkan.h>

namespace Stellar {
	class VulkanTexture : public Texture2D {
	public:
		VulkanTexture(const std::string& filePath, const TextureSpecification& spec);
		VulkanTexture(const TextureSpecification& spec, const void* data);
		~VulkanTexture() override;

		uint64_t getHash() const override;
		void generateMips() override;

		ImTextureID getImGuiTextureID() override;
		// Vulkan
		void invalidate();
		STLR_Ptr<Image2D> getImage() const;
		VkDescriptorSet getDescriptorSets() { return m_DescriptorSet; }
		VkDescriptorSet getImGuiDescriptorSets() { return m_ImGuiDescriptorSet; };
	protected:
		virtual bool loadImage(const std::string& filePath) override;
	private:
		TextureSpecification m_Specification;
		unsigned char* m_Pixels{};
		VkDeviceSize m_ImageSize{};
		VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
		VkDescriptorSet m_ImGuiDescriptorSet = VK_NULL_HANDLE;
	};
}