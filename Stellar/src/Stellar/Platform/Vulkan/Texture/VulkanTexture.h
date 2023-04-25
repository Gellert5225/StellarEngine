#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Texture.h"

#include <vulkan/vulkan.h>

namespace Stellar {
	class VulkanTexture : public Texture2D {
	public:
		VulkanTexture(const std::string& filePath);
		VulkanTexture(ImageFormat format, uint32_t width, uint32_t height, const void* data);
		~VulkanTexture() override;

		uint64_t getHash() const override;
		// Vulkan
		void invalidate();
		STLR_Ptr<Image2D> getImage() const;
		VkDescriptorSet getDescriptorSets() { return m_DescriptorSet; }
		VkDescriptorSet getImGuiDescriptorSets() { return m_ImGuiDescriptorSet; };
	protected:
		virtual bool loadImage(const std::string& filePath) override;
	private:
		uint32_t m_Width{};
		uint32_t m_Height{};
		unsigned char* m_Pixels{};
		VkDeviceSize m_ImageSize{};
		VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
		VkDescriptorSet m_ImGuiDescriptorSet = VK_NULL_HANDLE;
	};
}