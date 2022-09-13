#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include "Stellar/Application.h"

namespace Stellar {

    void Camera::recalculateViewMatrix() {
        glm::mat4 tranform = glm::translate(glm::mat4(1.0f), m_Position) *
                glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0, 0, 1));

        m_ViewMatrix = glm::inverse(tranform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void Camera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
        m_ProjectionMatrix = glm::ortho(left, right, bottom, top, near, far);
        m_ProjectionMatrix[1][1] *= -1;
    }

    void Camera::setPerspectiveProjection(float fovy, float aspect, float near, float far) {
        m_ProjectionMatrix = glm::perspective(fovy, aspect, near, far);
        m_ProjectionMatrix[1][1] *= -1;
    }
}