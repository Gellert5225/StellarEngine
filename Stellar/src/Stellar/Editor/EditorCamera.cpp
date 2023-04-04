#include "stlrpch.h"

#include "EditorCamera.h"

#include "Stellar/Core/Input.h"
#include "Stellar/Core/KeyCodes.h"

namespace Stellar {
	EditorCamera::EditorCamera(const float fov, const float aspect, const float near, const float far)
		: Camera(fov, aspect, near, far), m_VerticalFOV(fov), m_AspectRatio(aspect), m_NearClip(near), m_FarClip(far) {
		recalculateViewMatrix();
	}

	void EditorCamera::recalculateViewMatrix() {
		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(front);

		m_Right = glm::normalize(glm::cross(m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));
		m_Up    = glm::normalize(glm::cross(m_Right, m_Front));

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
		//m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void EditorCamera::onUpdate(const Timestep ts) {
		if (Input::IsKeyPressed(STLR_KEY_W))
			m_Position += m_Front * m_Speed * float(ts);
		else if (Input::IsKeyPressed(STLR_KEY_S))
			m_Position -=m_Front * m_Speed * float(ts);
		if (Input::IsKeyPressed(STLR_KEY_A))
			m_Position -= m_Right * m_Speed * float(ts);
		else if (Input::IsKeyPressed(STLR_KEY_D))
			m_Position += m_Right * m_Speed * float(ts);
		if (Input::IsKeyPressed(STLR_KEY_SPACE))
			m_Position.y -= m_Speed * ts;
		else if (Input::IsKeyPressed(STLR_KEY_LEFT_CONTROL))
			m_Position.y += m_Speed * ts;
		if (Input::IsKeyPressed(STLR_KEY_LEFT))
			setYaw(-m_Speed * ts * 40);
		else if (Input::IsKeyPressed(STLR_KEY_RIGHT))
			setYaw(m_Speed * ts * 40);
		if (Input::IsKeyPressed(STLR_KEY_UP))
			setPitch(-m_Speed * ts * 40);
		else if (Input::IsKeyPressed(STLR_KEY_DOWN))
			setPitch(m_Speed * ts * 40);

		recalculateViewMatrix();
	}

	void EditorCamera::SetViewportSize(uint32_t width, uint32_t height) {
		if (m_ViewportWidth == width && m_ViewportHeight == height)
			return;
		setPerspectiveProjection(m_VerticalFOV, (float)width / (float)height, m_NearClip, m_FarClip);
		m_ViewportWidth = width;
		m_ViewportHeight = height;
	}
} 