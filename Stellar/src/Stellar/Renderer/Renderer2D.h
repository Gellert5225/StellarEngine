#pragma once

#include "Camera.h"
#include "Texture.h"
#include "Stellar/Core/Core.h"

namespace Stellar {
	class STLR_API Renderer2D {
	public:
		static void Init();
		static void ShutDown();
		
		static void BeginScene(const Camera camera);
		static void EndScene();

		static void DrawQuad(const glm::vec3& position, 
							const glm::vec2& size, 
							const glm::vec4& color);

		static void DrawQuad(const glm::mat4& transform, 
							const glm::vec4& color,
							Ref<Texture2D> texture);
	};
}