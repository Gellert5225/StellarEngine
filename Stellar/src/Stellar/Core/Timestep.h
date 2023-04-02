#pragma once

#include <GLFW/glfw3.h>
#include "Stellar/Core/Core.h"

namespace Stellar {
	class STLR_API Timestep {
	public:
		explicit Timestep(float time = 0.0f) : m_Time(time) {}

		static float GetTime() { return (float)glfwGetTime(); }

		[[nodiscard]] float getSeconds() const { return m_Time; }
		[[nodiscard]] float getMilliseconds() const { return m_Time * 1000.f; }

		operator float() const { return m_Time; }
	private:
		float m_Time;
	};
}