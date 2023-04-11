#include "stlrpch.h"
#include "Camera.h"
#include "RendererAPI.h"

namespace Stellar {
	Camera::Camera(const float fov, const float width, const float height, const float near, const float far) {
		m_ProjectionMatrix = glm::perspectiveFov(glm::radians(fov), width, height, near, far);
	}


	void Camera::setPerspectiveProjection(float fov, float width, float height, float near, float far) {
		m_ProjectionMatrix = glm::perspectiveFov(glm::radians(fov), width, height, near, far);
	}

	void Camera::setOrthographicProjection(float width, float height, float nearP, float farP) {
		m_ProjectionMatrix = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, nearP, farP);
	}
}