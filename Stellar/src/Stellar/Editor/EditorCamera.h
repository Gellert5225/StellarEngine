#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Camera.h"
#include "Stellar/Core/Timestep.h"
#include "Stellar/Events/Event.h"

namespace Stellar {
	class STLR_API EditorCamera : public Camera {
	public:
		EditorCamera(const float fovy, const float aspect, const float near, const float far);

		void onUpdate(const Timestep ts);
		void onEvent(Event& e);

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

		[[nodiscard]] const glm::vec3& getPosition() const { return m_Position; }
        [[nodiscard]] const glm::vec3& getRight() const { return m_Right; }
        [[nodiscard]] const glm::vec3& getFront() const { return m_Front; }
        [[nodiscard]] float getRotation() const { return m_Rotation; }
	private:
	 	void recalculateViewMatrix();
	private:
        glm::vec3 m_Front;
        glm::vec3 m_Up{ 0.0f, 1.0f, 0.0f };
        glm::vec3 m_Right{};

        float m_Yaw = 90.0f; // camera pointing to positive Z
        float m_Pitch = 0.0f;
        float m_Rotation = 0.0f;
		 float m_Speed = 1.0f;
	};
}