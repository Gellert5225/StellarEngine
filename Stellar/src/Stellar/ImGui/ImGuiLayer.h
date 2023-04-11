#pragma once

#include "Stellar/Core/Layer.h"

#include "Stellar/Renderer/FrameBuffer.h"
#include "Stellar/Renderer/Texture.h"
#include <imgui.h>

namespace Stellar {
	class STLR_API ImGuiLayer: public Layer {
	public:
		virtual void begin() = 0;
		virtual void end() = 0;

		static ImGuiLayer* Create();
	protected:
		float m_Time = 0.0f;

	};
}

namespace Stellar::UI {
	void SetInputEnabled(bool enable);

	void* Image(Texture2D* texture, const ImVec2& size);
	void ImageFromFB(Ref<FrameBuffer> frameBuffer, const ImVec2& size);
}