#pragma once

#include "stlrpch.h"

#include "Core.h"
#include "Events/Event.h"

#include <GLFW/glfw3.h>

#include <utility>

namespace Stellar {
    struct WindowProperty {
        std::string title;
        unsigned int width;
        unsigned int height;

        explicit WindowProperty(std::string title = "Stellar Engine",
                                unsigned int width = 1280,
                                unsigned int height = 720)
            : title(std::move(title)), width(width), height(height) {}
    };

    class STLR_API Window {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        explicit Window(const WindowProperty& property);
        ~Window();

        virtual void onUpdate();
        virtual void setEventCallback(const EventCallbackFn& callback);
        virtual void setVsync(bool enabled);

        [[nodiscard]] virtual unsigned int getWidth() const;
        [[nodiscard]] virtual unsigned int getHeight() const;

        [[nodiscard]] virtual bool isVsync() const;

        [[nodiscard]] virtual GLFWwindow* getGLFWWindow() const;

        [[nodiscard]] bool wasWindowResized() const { return m_Data.frameBufferResized; }
        void resetWindowResizedFlag() { m_Data.frameBufferResized = false; }

        static Window* Create(const WindowProperty& property = WindowProperty());
    private:
        GLFWwindow* m_Window{};

        struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;
            bool frameBufferResized = false;

			EventCallbackFn EventCallback;
		};
        WindowData m_Data;

    private:
        virtual void init(const WindowProperty& property);
        virtual void shutDown();
    };
}