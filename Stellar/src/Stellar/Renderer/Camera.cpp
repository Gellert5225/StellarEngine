#include "Camera.h"

namespace Stellar {
    void Camera::recalculateViewMatrix() {
        m_ViewMatrix = glm::lookAt(m_Position,
                                   glm::vec3(m_Position.x, m_Position.y, 1.0f),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
        m_ProjectionMatrix = glm::perspective(fovy, aspect, near, far);
        //m_ProjectionMatrix[0][0] *= -1;
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void Camera::setOrthographicProjection(float width, float height, float nearP, float farP) {
        m_ProjectionMatrix = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, nearP, farP);
        //m_ProjectionMatrix[1][1] *= -1;
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    Camera::Camera() {
        m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
        m_Rotation = 0.0f;
        m_ViewMatrix = glm::lookAt(m_Position,
                                   glm::vec3(m_Position.x, m_Position.y, 1.0f),
                                   glm::vec3(0.0f, 1.0f, 0.0f));
    }

    void Camera::setOrtho(float left, float right, float top, float bottom, float near, float far) {
        m_ProjectionMatrix = glm::mat4{1.0f};
        m_ProjectionMatrix[0][0] = 2.f / (right - left);
        m_ProjectionMatrix[1][1] = 2.f / (bottom - top);
        m_ProjectionMatrix[2][2] = 1.f / (far - near);
        m_ProjectionMatrix[3][0] = -(right + left) / (right - left);
        m_ProjectionMatrix[3][1] = -(bottom + top) / (bottom - top);
        m_ProjectionMatrix[3][2] = -near / (far - near);
        //m_ProjectionMatrix[1][1] *= -1;

        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}