#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Stellar {
    struct STLR_API TransformComponent {
        glm::mat4 transform = glm::mat4(1.0f);

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::mat4& transform) : transform(transform) {}
        operator const glm::mat4&() const { return transform; }
        operator glm::mat4&() { return transform; }
    };

    struct STLR_API SpriteRendererComponent {
        glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
        Texture2D* texture;

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;
        SpriteRendererComponent(const glm::vec4& color, Texture2D* texture) 
            : color(color), texture(texture) {}
    };

    struct STLR_API MeshComponent {
        bool data;
        MeshComponent() = default;
    };
}