#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Texture.h"
#include "Stellar/Renderer/Camera.h"
#include "Stellar/Editor/EditorCamera.h"

#include <glm/glm.hpp>
#include <string>

namespace Stellar {
    struct STLR_API TagComponent {
        std::string tag;
        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag) : tag(tag) {}
    };

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

    struct STLR_API CameraComponent {
        Camera camera;
        bool primary = true;
        bool fixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

	struct STLR_API EditorCameraComponent {
        EditorCamera camera;
        bool primary = true;
        bool fixedAspectRatio = false;

        EditorCameraComponent() = default;
        EditorCameraComponent(const EditorCameraComponent&) = default;
    };
}