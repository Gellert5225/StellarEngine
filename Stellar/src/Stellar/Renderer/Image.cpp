#include "stlrpch.h"
#include "Image.h"

#if defined(__linux__) || defined(_WIN64)
#include "Stellar/Platform/Vulkan/Image/VulkanImage.h"
#endif

#include "Stellar/Renderer/RendererAPI.h"

#include "Stellar/Core/Log.h"

namespace Stellar {
	STLR_Ptr<Image2D> Image2D::Create(ImageSpecification specification) {
		switch (RendererAPI::Current()) {
			case RendererAPIType::Vulkan:
			#if defined(__linux__) || defined(_WIN64)
				return STLR_Ptr<VulkanImage2D>::Create(specification);
			#endif
			case RendererAPIType::Metal:
				STLR_CORE_ASSERT(false, "Metal Image is not yet supported");
			case RendererAPIType::None:
				break;
		}
		STLR_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}