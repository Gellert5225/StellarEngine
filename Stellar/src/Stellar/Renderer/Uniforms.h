#pragma once

#include "Stellar/Core.h"

#include <glm/glm.hpp>

namespace Stellar {
    struct STLR_API GlobalUniforms {
        glm::mat4 viewProjection{1.f};
        glm::mat4 model{1.f};
    };
}