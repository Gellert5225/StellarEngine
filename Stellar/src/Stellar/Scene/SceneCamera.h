#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/Camera.h"

namespace Stellar {
    class SceneCamera : public Camera {
    public:
        enum class ProjectionType { 
            Perspective = 0, Orthographic = 1 
        };

        SceneCamera();
        virtual ~SceneCamera();

        void setPerspective(float FOV, float near = 0.1f, float far = 1000.0f);
        void setOrthographic(float size, float near = -1.0f, float far = 1.0f);

        void setViewportSize(uint32_t width, uint32_t height);
    private:
        ProjectionType m_ProjectionType = ProjectionType::Perspective;

        float m_OrthoSize = 10.0f;
        float m_OrthoNear = -1.0f;
        float m_OrthoFar = 1.0f;

        float m_PerspectiveFOV = 45.0f;
        float m_PerspectiveNear = 0.1f;
        float m_PerspectiveFar = 100.0f;
    };
}