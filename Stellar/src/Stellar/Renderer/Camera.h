#pragma once

#include "Stellar/Core/Core.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace Stellar {
	class STLR_API Camera {
	public:
		Camera() = default;
		Camera(const float fov, const float aspect, const float near, const float far);

		void setOrthographicProjection(float width, float height, float nearP, float farP);
		void setPerspectiveProjection(float fov, float aspect, float near, float far);

		void setProjectionMatrix(const glm::mat4& projection) {
			m_ProjectionMatrix = projection;
		}

		[[nodiscard]] const glm::mat4& getViewmatrix() const { return m_ViewMatrix; }
		[[nodiscard]] const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
		[[nodiscard]] const glm::mat4& getViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

		// [[nodiscard]] const glm::vec3& getPosition() const { return m_Position; }
		// [[nodiscard]] const glm::vec3& getRight() const { return m_Right; }
		// [[nodiscard]] const glm::vec3& getFront() const { return m_Front; }
		// [[nodiscard]] float getRotation() const { return m_Rotation; }
	protected:
		glm::mat4 m_ProjectionMatrix{};
		glm::mat4 m_ViewMatrix{};
		glm::mat4 m_ViewProjectionMatrix{};
		glm::vec3 m_Position{0.0f, -1.0f, -2.0f};
	};
}