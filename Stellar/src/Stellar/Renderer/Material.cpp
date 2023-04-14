#include "stlrpch.h"

#include "Material.h"

#include "Stellar/Renderer/RendererAPI.h"
#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Material/VulkanMaterial.h"
#endif

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/Material/MetalMaterial.h"
#endif
#include "Stellar/Core/Log.h"

namespace Stellar {
	STLR_Ptr<Material> Material::Create(const STLR_Ptr<Shader>& shader, const std::string& name) {
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan:
			#if defined(__linux__) || defined(_WIN64)
				return STLR_Ptr<VulkanMaterial>::Create(shader, name);
			#endif
			case RendererAPIType::Metal:
			#if defined(__APPLE__)
				return new MetalMaterial(shader, name);
			#endif
			case RendererAPIType::None:
				break;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}