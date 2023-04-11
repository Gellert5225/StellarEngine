#include "stlrpch.h"

#include "EditorCamera.h"

#include "Stellar/Core/Input.h"
#include "Stellar/Core/KeyCodes.h"
#include "Stellar/ImGui/ImGuiLayer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Stellar {
	static void DisableMouse() {
		Input::SetCursorMode(CursorMode::Locked);
		UI::SetInputEnabled(false);
	}

	static void EnableMouse() {
		Input::SetCursorMode(CursorMode::Normal);
		UI::SetInputEnabled(true);
	}

	EditorCamera::EditorCamera(const float fov, const float width, const float height, const float near, const float far)
		: Camera(fov, width, height, near, far), m_FocalPoint(0.0f), m_VerticalFOV(fov), m_NearClip(near), m_FarClip(far) {
		constexpr glm::vec3 position = { -5, 5, 5 };
		m_Distance = glm::distance(position, m_FocalPoint);

		m_Yaw = -glm::radians(45.0f);
		m_Pitch = glm::radians(45.0f);

		m_Position = calculatePosition();
		const glm::quat orientation = getOrientation();
		m_Direction = glm::eulerAngles(orientation) * (180.0f / glm::pi<float>());
		m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		m_ViewMatrix = glm::inverse(m_ViewMatrix);
	}

	void EditorCamera::recalculateViewMatrix() {
		const float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;

		// Extra step to handle the problem when the camera direction is the same as the up vector
		const float cosAngle = glm::dot(getForwardDirection(), getUpDirection());
		if (cosAngle * yawSign > 0.99f)
			m_PitchDelta = 0.f;

		const glm::vec3 lookAt = m_Position + getForwardDirection();
		m_Direction = glm::normalize(lookAt - m_Position);
		m_Distance = glm::distance(m_Position, m_FocalPoint);
		m_ViewMatrix = glm::lookAt(m_Position, lookAt, glm::vec3{ 0.f, yawSign, 0.f });

		//damping for smooth camera
		m_YawDelta *= 0.6f;
		m_PitchDelta *= 0.6f;
		m_PositionDelta *= 0.8f;
	}

	void EditorCamera::onUpdate(const Timestep ts) {
		const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
		const glm::vec2 delta = (mouse - m_InitialMousePosition) * 0.002f;

		if (Input::IsMouseButtonPressed(STLR_MOUSE_RIGHT) && !Input::IsKeyPressed(STLR_KEY_LEFT_ALT)) {
			m_CameraMode = CameraMode::FLYCAM;
			DisableMouse();
			const float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;

			const float speed = getCameraSpeed();

			if (Input::IsKeyPressed(STLR_KEY_Q))
				m_PositionDelta -= ts.getMilliseconds() * speed * glm::vec3{ 0.f, yawSign, 0.f };
			if (Input::IsKeyPressed(STLR_KEY_E))
				m_PositionDelta += ts.getMilliseconds() * speed * glm::vec3{ 0.f, yawSign, 0.f };
			if (Input::IsKeyPressed(STLR_KEY_S))
				m_PositionDelta -= ts.getMilliseconds() * speed * m_Direction;
			if (Input::IsKeyPressed(STLR_KEY_W))
				m_PositionDelta += ts.getMilliseconds() * speed * m_Direction;
			if (Input::IsKeyPressed(STLR_KEY_A))
				m_PositionDelta -= ts.getMilliseconds() * speed * m_RightDirection;
			if (Input::IsKeyPressed(STLR_KEY_D))
				m_PositionDelta += ts.getMilliseconds() * speed * m_RightDirection;

			constexpr float maxRate{ 0.12f };
			m_YawDelta += glm::clamp(yawSign * delta.x * rotationSpeed() / 3.0f, -maxRate, maxRate);
			m_PitchDelta += glm::clamp(delta.y * rotationSpeed() / 3.0f, -maxRate, maxRate);

			m_RightDirection = glm::cross(m_Direction, glm::vec3{ 0.f, yawSign, 0.f });

			m_Direction = glm::rotate(glm::normalize(glm::cross(glm::angleAxis(-m_PitchDelta, m_RightDirection),
				glm::angleAxis(-m_YawDelta, glm::vec3{ 0.f, yawSign, 0.f }))), m_Direction);

			const float distance = glm::distance(m_FocalPoint, m_Position);
			m_FocalPoint = m_Position + getForwardDirection() * distance;
			m_Distance = distance;
		} else if (Input::IsKeyPressed(STLR_KEY_LEFT_ALT)) {
			m_CameraMode = CameraMode::ARCBALL;

			if (Input::IsMouseButtonPressed(STLR_MOUSE_MIDDLE)) {
				DisableMouse();
				mousePan(delta);
			} else if (Input::IsMouseButtonPressed(STLR_MOUSE_LEFT)) {
				DisableMouse();
				mouseRotate(delta);
			} else if (Input::IsMouseButtonPressed(STLR_MOUSE_RIGHT)) {
				DisableMouse();
				mouseZoom(delta.x + delta.y);
			} else {
				EnableMouse();
			}
		}else {
			EnableMouse();
		}

		m_InitialMousePosition = mouse;
		m_Position += m_PositionDelta;
		m_Yaw += m_YawDelta;
		m_Pitch += m_PitchDelta;

		if (m_CameraMode == CameraMode::ARCBALL)
			m_Position = calculatePosition();

		recalculateViewMatrix();
	}

	void EditorCamera::SetViewportSize(uint32_t width, uint32_t height) {
		if (m_ViewportWidth == width && m_ViewportHeight == height)
			return;
		setPerspectiveProjection(m_VerticalFOV, (float)width, (float)height, m_NearClip, m_FarClip);
		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}

	glm::vec3 EditorCamera::getUpDirection() const {
		return glm::rotate(getOrientation(), glm::vec3(0.0f, -1.0f, 0.0f));
	}

	glm::vec3 EditorCamera::getRightDirection() const {
		return glm::rotate(getOrientation(), glm::vec3(-1.0f, 0.0f, 0.0f));
	}

	glm::vec3 EditorCamera::getForwardDirection() const {
		return glm::rotate(getOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::quat EditorCamera::getOrientation() const {
		return glm::quat(glm::vec3(m_Pitch + m_PitchDelta, -m_Yaw - m_YawDelta, 0.0f));
	}

	void EditorCamera::mousePan(const glm::vec2& delta) {
		auto [xSpeed, ySpeed] = panSpeed();
		m_FocalPoint += -getRightDirection() * delta.x * xSpeed * m_Distance;
		m_FocalPoint += getUpDirection() * delta.y * ySpeed * m_Distance;
	}

	void EditorCamera::mouseRotate(const glm::vec2& delta) {
		float yawSign = getUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * rotationSpeed();
		m_Pitch += delta.y * rotationSpeed();
	}
	void EditorCamera::mouseZoom(float delta) {
		m_Distance -= delta * zoomSpeed();
		if (m_Distance < 1.0f) {
			m_FocalPoint += getForwardDirection();
			m_Distance = 1.0f;
		}
	}

	glm::vec3 EditorCamera::calculatePosition() const {
		return m_FocalPoint - getForwardDirection() * m_Distance + m_PositionDelta;
	}

	std::pair<float, float> EditorCamera::panSpeed() const {
		float x = std::min(m_ViewportWidth / 1000.0f, 2.4f); // max = 2.4f
		float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

		float y = std::min(m_ViewportHeight / 1000.0f, 2.4f); // max = 2.4f
		float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

		return { xFactor, yFactor };
	}

	float EditorCamera::rotationSpeed() const {
		return 0.8f;
	}

	float EditorCamera::zoomSpeed() const {
		float distance = m_Distance * 0.2f;
		distance = std::max(distance, 0.0f);
		float speed = distance * distance;
		speed = std::min(speed, 100.0f); // max speed = 100
		return speed;
	}

	float EditorCamera::getCameraSpeed() const {
		float speed = m_NormalSpeed;
		if (Input::IsKeyPressed(STLR_KEY_LEFT_CONTROL))
			speed /= 2 - glm::log(m_NormalSpeed);
		if (Input::IsKeyPressed(STLR_KEY_LEFT_SHIFT))
			speed *= 2 - glm::log(m_NormalSpeed);

		return glm::clamp(speed, MIN_SPEED, MAX_SPEED);
	}
} 