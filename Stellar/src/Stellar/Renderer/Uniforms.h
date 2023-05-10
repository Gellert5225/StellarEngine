#pragma once

#include <glm/glm.hpp>

namespace Stellar {
	struct GlobalUniforms {
		alignas(16) glm::mat4 viewProjection{1.f};
	};

	struct Push {
		alignas(16) glm::mat4 model{1.f};
	};

	struct InstanceData {
		alignas(16) glm::mat4 transform{1.f};
		alignas(16) glm::vec4 color;
	};
}