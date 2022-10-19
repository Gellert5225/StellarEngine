#include "stlrpch.h"
#include "Camera.h"
#include "RendererAPI.h"

namespace Stellar {
    void Camera::recalculateViewMatrix() {
        glm::vec3 front;
        front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        front.y = sin(glm::radians(m_Pitch));
        front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
        m_Front = glm::normalize(front);

        m_Right = glm::normalize(glm::cross(m_Front, glm::vec3(0.0f, 1.0f, 0.0f)));
        m_Up    = glm::normalize(glm::cross(m_Right, m_Front));

        m_ViewMatrix = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
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

    Camera::Camera() {
       recalculateViewMatrix();
    }
}