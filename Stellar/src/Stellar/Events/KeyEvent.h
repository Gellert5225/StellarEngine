#pragma once

#include "Event.h"

namespace Stellar {
    class STLR_API KeyEvent : public Event {
    public:
        [[nodiscard]] inline int getKeyCode() const { return m_KeyCode; }
        EVENT_CLASS_CATEGORY(Keyboard | Input)
    protected:
        explicit KeyEvent(int keyCode)
            : m_KeyCode(keyCode) {}
        int m_KeyCode;
    };

    class STLR_API KeyPressedEvent : public KeyEvent {
    public:
        KeyPressedEvent(int keyCode, int repeatCount)
            : KeyEvent(keyCode), m_RepeatCount(repeatCount) {}

        [[nodiscard]] inline int getRepeatCount() const { return m_RepeatCount; }
        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << " repeats)";
            return ss.str();
        }
        EVENT_CLASS_TYPE(KeyPressed)
    private:
        int m_RepeatCount;
    };

    class STLR_API KeyReleasedEvent : public KeyEvent {
    public:
        explicit KeyReleasedEvent(int keyCode)
            : KeyEvent(keyCode) {}

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "KeyReleasedEvent: " << m_KeyCode;
            return ss.str();
        }

        EVENT_CLASS_TYPE(KeyReleased)
    };
}