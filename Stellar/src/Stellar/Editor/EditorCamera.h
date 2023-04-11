#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Camera.h"
#include "Stellar/Core/Timestep.h"
#include "Stellar/Events/Event.h"

namespace Stellar {
	enum class CameraMode {
		NONE, FLYCAM, ARCBALL
	};

	class STLR_API EditorCamera : public Camera {
	public:
		EditorCamera(const float fov, const float width, const float height, const float near, const float far);

		void onUpdate(const Timestep ts);
		void onEvent(Event& e);

		void setPosition(const glm::vec3 position) { m_Position = position; }

		void setViewportSize(uint32_t width, uint32_t height);

		[[nodiscard]] const glm::vec3& getPosition() const { return m_Position; }
		[[nodiscard]] float getCameraSpeed() const;

		bool isActive() const { return m_IsActive; }
		void setActive(bool active) { m_IsActive = active; }
		
		glm::vec3 getUpDirection() const;
		glm::vec3 getRightDirection() const;
		glm::vec3 getForwardDirection() const;
		glm::quat getOrientation() const;

	private:
		void recalculateViewMatrix();

		void mousePan(const glm::vec2& delta);
		void mouseRotate(const glm::vec2& delta);
		void mouseZoom(float delta);

		glm::vec3 calculatePosition() const;

		std::pair<float, float> panSpeed() const;
		float rotationSpeed() const;
		float zoomSpeed() const;
	private:
		glm::vec3 m_Direction, m_FocalPoint;
		glm::vec3 m_InitialFocalPoint, m_InitialRotation;
		glm::vec3 m_PositionDelta{};
		glm::vec3 m_RightDirection{};
		glm::vec2 m_InitialMousePosition{};

		float m_Distance = 10.0f;
		float m_NormalSpeed{ 0.002f };
		float m_PitchDelta{}, m_YawDelta{};

		float m_Yaw = 90.0f; // camera pointing to positive Z
		float m_Pitch = 0.0f;
		float m_VerticalFOV = glm::radians(60.0f);
		float m_AspectRatio = 1.0f;
		float m_NearClip = 0.01f;
		float m_FarClip = 100.0f;
		constexpr static float MIN_SPEED{ 0.0005f }, MAX_SPEED{ 2.0f };

		uint32_t m_ViewportWidth{ 1280 }, m_ViewportHeight{ 720 };

		bool m_IsActive = false;

		CameraMode m_CameraMode{ CameraMode::ARCBALL };
	};
}