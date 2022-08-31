#pragma once

#include "Event.h"

namespace Stellar {
    class STLR_API KeyEvent : public Event {
    public:
        [[nodiscard]] inline unsigned int getKeyCode() const { return m_KeyCode; }
        EVENT_CLASS_CATEGORY(Keyboard | Input)
    protected:
        explicit KeyEvent(unsigned int keyCode)
            : m_KeyCode(keyCode) {}
        unsigned int m_KeyCode;
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

    class STLR_API KeyTypedEvent : public KeyEvent {
    public:
        explicit KeyTypedEvent(unsigned int keyCode)
                : KeyEvent(keyCode) {}

        [[nodiscard]] std::string toString() const override {
            std::stringstream ss;
            ss << "KeyTypedEvent: " << m_KeyCode;
            return ss.str();
        }
        EVENT_CLASS_TYPE(KeyTyped)
    };
}