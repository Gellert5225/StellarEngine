#pragma once

#include "Stellar/Core/Core.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace Stellar {
    class STLR_API Camera {
    public:
        Camera();
        void setOrthographicProjection(float width, float height, float nearP, float farP);
        void setPerspectiveProjection(float fovy, float aspect, float near, float far);

        void setPosition(const glm::vec3 position) { 
            m_Position = position; 
            recalculateViewMatrix(); 
        }
        void setRotation(float rotation) { m_Rotation = rotation; recalculateViewMatrix(); }
        void setYaw(float yaw) { m_Yaw += yaw; recalculateViewMatrix(); }
        void setPitch(float pitch) { 
            m_Pitch += pitch; 
            if (m_Pitch > 89.0f)
                m_Pitch = 89.0f;
            if (m_Pitch < -89.0f)
                m_Pitch = -89.0f;
            recalculateViewMatrix(); 
        }

        [[nodiscard]] const glm::mat4& getViewmatrix() const { return m_ViewMatrix; }
        [[nodiscard]] const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
        [[nodiscard]] const glm::mat4& getViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

        [[nodiscard]] const glm::vec3& getPosition() const { return m_Position; }
        [[nodiscard]] const glm::vec3& getRight() const { return m_Right; }
        [[nodiscard]] const glm::vec3& getFront() const { return m_Front; }
        [[nodiscard]] float getRotation() const { return m_Rotation; }
    private:
        glm::mat4 m_ProjectionMatrix{};
        glm::mat4 m_ViewMatrix{};
        glm::mat4 m_ViewProjectionMatrix{};

        glm::vec3 m_Position{};
        glm::vec3 m_Front;
        glm::vec3 m_Up{ 0.0f, 1.0f, 0.0f };
        glm::vec3 m_Right{};

        float m_Yaw = 90.0f; // camera pointing to positive Z
        float m_Pitch = 0.0f;
        float m_Rotation = 0.0f;

        void recalculateViewMatrix();
    };
}