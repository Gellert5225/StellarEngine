#pragma once

#include "stlrpch.h"

#include "Core.h"
#include "Events/Event.h"
#include "Platform/Vulkan/Renderer/VulkanRendererContext.h"

#include "Platform/Vulkan/SwapChain/SwapChain.h"

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

        static Window* Create(const WindowProperty& property = WindowProperty());
    private:
        GLFWwindow* m_Window{};

        RendererContext* m_Context;
        // TODO: Abstract this into different platforms
        SwapChain* m_SwapChain;

        struct WindowData {
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};
        WindowData m_Data;

    private:
        virtual void init(const WindowProperty& property);
        virtual void shutDown();
    };
}