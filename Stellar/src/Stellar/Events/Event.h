#pragma once

#include "stlrpch.h"
#include "Stellar/Core.h"

namespace Stellar {
    enum class EventType {
        None = 0,
        WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
        AppTick, AppUpdate, AppRender,
        KeyPressed, KeyReleased, KeyTyped,
        MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
    };

    enum EventCategory {
        None = 0,
        App            = BIT(0),
        Input          = BIT(1),
        Keyboard       = BIT(2),
        Mouse          = BIT(3),
        MouseButton    = BIT(4)
    };

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::type; }\
								virtual EventType getEventType() const override { return GetStaticType(); }\
								virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return category; }

    class STLR_API Event {
        friend class EventDispatcher;
    public:
        bool handled = false;

        [[nodiscard]] virtual EventType getEventType() const = 0;
        [[nodiscard]] virtual const char* getName() const = 0;
        [[nodiscard]] virtual int getCategoryFlags() const = 0;
        [[nodiscard]] virtual std::string toString() const { return getName(); }

        [[nodiscard]] inline bool isInCategory(EventCategory category) const {
            return getCategoryFlags() & category;
        }
    };

    class EventDispatcher {
        template<typename T>
        using EventFn = std::function<bool(T&)>;
    public:
        explicit EventDispatcher(Event& event)
            : m_Event(event) {}

        template<typename T>
        bool dispatch(EventFn<T> func) {
            if (m_Event.getEventType() == T::GetStaticType()) {
                m_Event.handled = func(*(T*)&m_Event);
                return true;
            }
            return false;
        }
    private:
        Event& m_Event;
    };

    inline std::ostream& operator<<(std::ostream& os, const Event& e) {
        return os << e.toString();
    }
}