#pragma once

#include "Stellar/Core/Core.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace Stellar {
	class Camera {
	public:
		Camera() = default;
		Camera(const float fov, const float width, const float height, const float near, const float far);

		void setOrthographicProjection(float width, float height, float nearP, float farP);
		void setPerspectiveProjection(float fov, float width, float height, float near, float far);

		void setProjectionMatrix(const glm::mat4& projection) {
			m_ProjectionMatrix = projection;
		}

		[[nodiscard]] const glm::mat4& getViewmatrix() const { return m_ViewMatrix; }
		[[nodiscard]] const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
		[[nodiscard]] const glm::mat4 getViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

		// [[nodiscard]] const glm::vec3& getPosition() const { return m_Position; }
		// [[nodiscard]] const glm::vec3& getRight() const { return m_Right; }
		// [[nodiscard]] const glm::vec3& getFront() const { return m_Front; }
		// [[nodiscard]] float getRotation() const { return m_Rotation; }
	protected:
		glm::mat4 m_ProjectionMatrix{1.0f};
		glm::mat4 m_ViewMatrix{1.0f};
		glm::vec3 m_Position{0.0f, 0.0f, -2.0f};
	};
}