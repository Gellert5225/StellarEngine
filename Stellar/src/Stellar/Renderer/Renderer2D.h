#pragma once

#include "Camera.h"

namespace Stellar {
    class Renderer2D {
    public:
        static void Init();
        static void ShutDown();
        
        static void BeginScene(const Camera camera);
        static void EndScene();

        static void DrawQuad(const glm::vec3& position, 
                             const glm::vec2& size, 
                             const glm::vec3& color);
    };
}