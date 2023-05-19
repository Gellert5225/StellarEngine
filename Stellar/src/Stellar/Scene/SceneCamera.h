#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Camera.h"

namespace Stellar {
	class SceneCamera : public Camera {
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };
	public:
		void setPerspective(float fov, float nearClip, float farClip);
		void setOrthographic(float size, float nearClip, float farClip);
		void setViewPortSize(uint32_t width, uint32_t height);

		void setProjectionType(ProjectionType type) { m_ProjectionType = type; }
		ProjectionType getProjectionType() const { return m_ProjectionType; }

		void setFov(const float fov) { m_Fov = fov; }
		float getFov() const { return m_Fov; }

		void setPerspectiveNear(const float nearClip) { m_PerspectiveNear = nearClip; }
		float getPerspectiveNear() const { return m_PerspectiveNear; }

		void setPerspectiveFar(const float farClip) { m_PerspectiveFar = farClip; }
		float getPerspectiveFar() const { return m_PerspectiveFar; }

		void setOrthoGraphicSize(const float size) { m_OrthoSize = size; }
		float getOrthoGraphicSize() const { return m_OrthoSize; };

		void setOrthographicNear(const float nearClip) { m_OrthoNear = nearClip; }
		float getOrthographicNear() const { return m_OrthoNear; }

		void setOrthographicFar(const float farClip) { m_OrthoFar = farClip; }
		float getOrthographicFar() const { return m_OrthoFar; }

	private:
		ProjectionType m_ProjectionType = ProjectionType::Perspective;

		float m_Fov = 60.0f;
		float m_PerspectiveNear = 0.1f, m_PerspectiveFar = 1000.0f;

		float m_OrthoSize = 10.0f;
		float m_OrthoNear = 0.1f, m_OrthoFar = 1.0f;
	};
}