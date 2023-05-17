#pragma once

#include <vulkan/vulkan.h>
#include <optional>

namespace Stellar {
	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		[[nodiscard]] bool isComplete() const {
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};
}

