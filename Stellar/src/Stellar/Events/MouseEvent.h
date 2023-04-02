#pragma once

#include "stlrpch.h"
#include "Event.h"

namespace Stellar {
	class STLR_API MouseMovedEvent : public Event {
	public:
		MouseMovedEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y) {}

		[[nodiscard]] inline float getX() const { return m_MouseX; }
		[[nodiscard]] inline float getY() const { return m_MouseY; }

		[[nodiscard]] std::string toString() const override {
			std::stringstream out;
			out << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return out.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(Mouse | Input)

	private:
		float m_MouseX, m_MouseY;
	};

	class STLR_API MouseScrolledEvent : public Event {
	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		[[nodiscard]] inline float getXOffset() const { return m_XOffset; }
		[[nodiscard]] inline float getYOffset() const { return m_YOffset; }

		[[nodiscard]] std::string toString() const override {
			std::stringstream out;
			out << "MouseScrolledEvent: " << m_XOffset << ", " << m_YOffset;
			return out.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(Mouse | Input)

	private:
		float m_XOffset, m_YOffset;
	};

	class STLR_API MouseButtonEvent : public Event {
	public:
		[[nodiscard]] inline int getMouseButton() const { return m_Button; }
		
		EVENT_CLASS_CATEGORY(Mouse | Input)

	protected:
		explicit MouseButtonEvent(int button)
			: m_Button(button) {
		}
		int m_Button;
	};

	class STLR_API MouseButtonPressedEvent : public MouseButtonEvent {
	public:
		explicit MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button) {
		}

		[[nodiscard]] std::string toString() const override {
			std::stringstream out;
			out << "MouseButtonPressedEvent: " << m_Button;
			return out.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class STLR_API MouseButtonReleasedEvent : public MouseButtonEvent {
	public:
		explicit MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) {
		}

		[[nodiscard]] std::string toString() const override {
			std::stringstream out;
			out << "MouseButtonReleasedEvent: " << m_Button;
			return out.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};
}