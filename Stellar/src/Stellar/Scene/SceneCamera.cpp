#include "stlrpch.h"
#include "SceneCamera.h"

namespace Stellar {
    void SceneCamera::setPerspective(float FOV, float near, float far) {
		m_ProjectionType = ProjectionType::Perspective;
		m_PerspectiveFOV = FOV;
		m_PerspectiveNear = near;
		m_PerspectiveFar = far;
	}

    void SceneCamera::setOrthographic(float size, float near, float far) {
		m_ProjectionType = ProjectionType::Orthographic;
		m_OrthoSize = size;
		m_OrthoNear = near;
		m_OrthoFar = far;
	}

    void SceneCamera::setViewportSize(uint32_t width, uint32_t height) {
        auto aspect = (float)width / (float)height;
        switch (m_ProjectionType) {
            case ProjectionType::Perspective:
                setPerspectiveProjection(glm::radians(m_PerspectiveFOV), aspect, m_PerspectiveNear, m_PerspectiveFar);
                break;
            case ProjectionType::Orthographic:
                float width = m_OrthoSize * aspect;
                float height = m_OrthoSize;
                setOrthographicProjection(width, height, m_OrthoNear, m_OrthoFar);
                break;
		}
    }
}