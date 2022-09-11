#include "OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Stellar {

    OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
    : m_ProjectionMatrix(glm::ortho(left, right, bottom, top)) {
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }

    void OrthographicCamera::recalculateViewMatrix() {
        glm::mat4 tranform = glm::translate(glm::mat4(1.0f), m_Position) *
                glm::rotate(glm::mat4(1.0f), m_Rotation, glm::vec3(0, 0, 1));

        m_ViewMatrix = glm::inverse(tranform);
        m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
    }
}