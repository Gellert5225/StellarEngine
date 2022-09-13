#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Stellar {
    void Camera::recalculateViewMatrix() {
        glm::mat4 tranform = glm::translate(glm::mat4(1.0f), m_Position) *
                glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0, 0, 1));

        m_ViewMatrix = glm::inverse(tranform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
        m_ProjectionMatrix = glm::perspective(fovy, aspect, near, far);
        m_ProjectionMatrix[1][1] *= -1;
    }

    void Camera::setOrthographicProjection(float width, float height, float nearP, float farP) {
        m_ProjectionMatrix = glm::ortho(-width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f, nearP, farP);
        m_ProjectionMatrix[1][1] *= -1;
    }
}