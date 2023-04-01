#include "stlrpch.h"
#include "Camera.h"
#include "RendererAPI.h"

namespace Stellar {
	Camera::Camera(const float fovy, const float aspect, const float near, const float far) {
		m_ProjectionMatrix = glm::perspective(fovy, aspect, near, far);
	}


    void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
        m_ProjectionMatrix = glm::perspective(fovy, aspect, near, far);
        m_ProjectionMatrix[1][1] *= -1;
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void Camera::setOrthographicProjection(float width, float height, float nearP, float farP) {
        m_ProjectionMatrix = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, nearP, farP);
        m_ProjectionMatrix[1][1] *= -1;
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}