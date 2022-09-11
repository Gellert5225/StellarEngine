#pragma once

#include "Stellar/Core.h"

#include <glm/glm.hpp>

namespace Stellar {
    class STLR_API OrthographicCamera {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);

        void setPosition(const glm::vec3 position) { m_Position = position; }
        void setRotation(float rotation) { m_Rotation = rotation; recalculateViewMatrix(); }

        [[nodiscard]] const glm::mat4& getViewmatrix() const { return m_ViewMatrix; }
        [[nodiscard]] const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
        [[nodiscard]] const glm::mat4& getViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

        [[nodiscard]] const glm::vec3& getPosition() const { return m_Position; }
        [[nodiscard]] float getRotation() const { return m_Rotation; }
    private:
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewMatrix{};
        glm::mat4 m_ViewProjectionMatrix{};

        glm::vec3 m_Position{};
        float m_Rotation = 0.0f;

        void recalculateViewMatrix();
    };
}