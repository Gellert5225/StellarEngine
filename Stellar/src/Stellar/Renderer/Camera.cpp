#include "stlrpch.h"
#include "Camera.h"
#include "RendererAPI.h"

namespace Stellar {
	Camera::Camera(const float fov, const float width, const float height, const float nearP, const float farP) {
		m_ProjectionMatrix = glm::perspectiveFov(glm::radians(fov), width, height, nearP, farP);
	}


	void Camera::setPerspectiveProjection(float fov, float width, float height, float nearP, float farP) {
		m_ProjectionMatrix = glm::perspectiveFov(glm::radians(fov), width, height, nearP, farP);
	}

	void Camera::setOrthographicProjection(float width, float height, float nearP, float farP) {
		m_ProjectionMatrix = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, nearP, farP);
	}
}