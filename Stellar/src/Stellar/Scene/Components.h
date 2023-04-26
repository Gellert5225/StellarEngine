#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Texture.h"
#include "Stellar/Renderer/Camera.h"
#include "Stellar/Scene/SceneCamera.h"
#include "Stellar/Editor/EditorCamera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <string>

namespace Stellar {
	struct TagComponent {
		std::string tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag) : tag(tag) {}
	};

	struct TransformComponent {
		glm::vec3 translation = { 0.0f, -1.0f, 1.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation)
			: translation(translation) {}

		glm::mat4 getTransform() const {
			return glm::translate(glm::mat4(1.0f), translation)
				* glm::toMat4(glm::quat(rotation))
				* glm::scale(glm::mat4(1.0f), scale);
		}
	};

	struct SpriteRendererComponent {
		glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
		STLR_Ptr<Texture2D> texture = Texture2D::Create(ImageFormat::RGBA);

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color, STLR_Ptr<Texture2D> texture) 
			: color(color), texture(texture) {}
	};

	struct MeshComponent {
		bool data;
		MeshComponent() = default;
	};

	struct CameraComponent {
		SceneCamera camera;
		bool primary = true;
		bool fixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct EditorCameraComponent {
		EditorCamera camera;
		bool primary = true;
		bool fixedAspectRatio = false;

		EditorCameraComponent() = default;
		EditorCameraComponent(const EditorCameraComponent&) = default;
	};
}