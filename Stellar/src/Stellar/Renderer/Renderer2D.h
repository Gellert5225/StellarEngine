#pragma once

#include "Camera.h"
#include "Texture.h"
#include "Stellar/Core/Core.h"
#include "Stellar/Editor/EditorCamera.h"

namespace Stellar {
	class STLR_API Renderer2D {
	public:
		static void Init();
		static void ShutDown();
		
		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void BeginScene(const EditorCamera& camera);
		static void EndScene();

		static void DrawQuad(const glm::vec3& position, 
							const glm::vec2& size, 
							const glm::vec4& color);

		static void DrawQuad(const glm::mat4& transform, 
							const glm::vec4& color,
							STLR_Ptr<Texture2D> texture = nullptr);

		static void DrawQuad(const glm::mat4& transform, 
							const glm::vec4& color,
							const STLR_Ptr<Texture2D>& texture, 
							float tilingFactor);
	private:
		static void FlushAndReset();
	};
}