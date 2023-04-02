#pragma once

#include "stlrpch.h"

#include "Stellar/Core/Core.h"
#include "Stellar/Events/Event.h"
#include "Stellar/Renderer/RendererContext.h"
#include "Stellar/Renderer/SwapChain.h"

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

        explicit Window(WindowProperty  property);
        virtual ~Window();

        virtual void init();
        virtual void onUpdate();
        virtual void setEventCallback(const EventCallbackFn& callback);
        virtual void setVsync(bool enabled);
        virtual void swapBuffers();

        [[nodiscard]] virtual unsigned int getWidth() const;
        [[nodiscard]] virtual unsigned int getHeight() const;

        [[nodiscard]] virtual bool isVsync() const;

        [[nodiscard]] virtual GLFWwindow* getGLFWWindow() const;

        [[nodiscard]] SwapChain* getSwapChain() const;
        [[nodiscard]] RendererContext* getRendererContext() const;

        static Scope<Window> Create(const WindowProperty& property = WindowProperty());
    private:
        GLFWwindow* m_Window{};

        RendererContext* m_Context = nullptr;
        // TODO: Abstract this into different platforms
        SwapChain* m_SwapChain = nullptr;

        struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};
        WindowData m_Data;
        WindowProperty m_Property;

    private:
        virtual void shutDown();
    };
}