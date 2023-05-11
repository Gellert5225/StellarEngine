#pragma once

#include "Stellar/Core/Layer.h"

#include "Stellar/Renderer/FrameBuffer.h"
#include "Stellar/Renderer/Texture.h"
#include <imgui.h>

namespace Stellar {
	class ImGuiLayer: public Layer {
	public:
		virtual void begin() = 0;
		virtual void end() = 0;
		virtual void onEvent(Event& e) = 0;

		virtual void blockEvents(bool block) { m_BlockEvents = block; }

		void setDarkThemeColors();

		static ImGuiLayer* Create();
	protected:
		float m_Time = 0.0f;
		bool m_BlockEvents = true;
	};
}

namespace Stellar::UI {
	void SetInputEnabled(bool enable);

	bool IsInputEnabled();

	void* Image(STLR_Ptr<Texture2D>, const ImVec2& size);
	void ImageFromFB(STLR_Ptr<FrameBuffer> frameBuffer, const ImVec2& size);

	bool ImageButton(STLR_Ptr<Texture2D> texture, const ImVec2& size, const ImVec2& uv0 = {0,0}, const ImVec2& uv1 = {1,1}, int frame_padding = -1, const ImVec4& bg_col = {0,0,0,0}, const ImVec4& tint_col = {1,1,1,1});
}