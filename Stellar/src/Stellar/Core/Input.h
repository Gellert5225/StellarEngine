#pragma once

#include "Core.h"
#include "KeyCodes.h"

namespace Stellar {

	class STLR_API Input {
	public:
		static bool IsKeyPressed(int keycode) { return s_Instance->isKeypressedImpl(keycode); }
		static bool IsMouseButtonPressed(int button) { return s_Instance->isMouseButtonPressedImpl(button); }
		static int GetMouseX() { return s_Instance->getMouseXImpl(); }
		static int GetMouseY() { return s_Instance->getMouseYImpl(); }
		static void SetCursorMode(CursorMode mode);
		static void Close() { delete s_Instance; }
		static CursorMode GetCursorMode();
	protected:
		virtual bool isKeypressedImpl(int);
		virtual bool isMouseButtonPressedImpl(int);
		virtual float getMouseXImpl();
		virtual float getMouseYImpl();
	private:
		static Input* s_Instance;
	};
}