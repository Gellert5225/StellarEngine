#include "stlrpch.h"

#include "SceneCamera.h"

namespace Stellar {
	void SceneCamera::setPerspective(float fov, float near, float far) {
		m_ProjectionType = ProjectionType::Perspective;
		m_Fov = fov;
		m_PerspectiveNear = near;
		m_PerspectiveFar = far;
	}

	void SceneCamera::setOrthographic(float size, float near, float far) {
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthoSize = size;
		m_OrthoNear = near;
		m_OrthoFar = far;
	}

	void SceneCamera::setViewPortSize(uint32_t width, uint32_t height) {
		switch (m_ProjectionType) {
		case ProjectionType::Perspective:
			setPerspectiveProjection(m_Fov, float(width), float(height), m_PerspectiveNear, m_PerspectiveFar);
			break;
		case ProjectionType::Orthographic:
			float aspect = float(width) / float(height);
			float w = m_OrthoSize * aspect;
			float h = m_OrthoSize;
			setOrthographicProjection(w, h, m_OrthoNear, m_OrthoFar);
			break;
		}
	}
}