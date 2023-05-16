#include "Texture.h"

#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Texture/VulkanTexture.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Texture/MetalTexture.h"
#endif

#include "Stellar/Core/Log.h"
#include "Stellar/Renderer/RendererAPI.h"

namespace Stellar {
	STLR_Ptr<Texture2D> Texture2D::Create(const std::string& filePath, const TextureSpecification& spec) {
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan:
			#if defined(__linux__) || defined(_WIN64)
				return STLR_Ptr<VulkanTexture>::Create(filePath, spec);
			#endif
			case RendererAPIType::Metal:
			#if defined(__APPLE__)
				return STLR_Ptr<MetalTexture>::Create(filePath, isImGuiTexture);
			#endif
			case RendererAPIType::None:
				break;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	STLR_Ptr<Texture2D> Texture2D::Create(const TextureSpecification& spec, const void* data) {
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan:
			#if defined(__linux__) || defined(_WIN64)
				return STLR_Ptr<VulkanTexture>::Create(spec, data);
			#endif
			case RendererAPIType::Metal:
			#if defined(__APPLE__)
				return STLR_Ptr<MetalTexture>::Create(format, width, height, data);;
			#endif
			case RendererAPIType::None:
				break;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	uint32_t Texture2D::GetMipCount(uint32_t width, uint32_t height) {
		return (uint32_t)std::floor(std::log2(glm::min(width, height))) + 1;
	}
}