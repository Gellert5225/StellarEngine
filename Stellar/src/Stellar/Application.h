#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"

#include "ImGUI/ImGuiLayer.h"

#include "Core/Timestep.h"

#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"

namespace Stellar {
    class STLR_API Application {
        struct AppInfo {
            std::string appName;
            std::string graphicsInfo;
            std::string vulkanVersion;
        };

    public:
        Application();
        virtual ~Application();

        void run();
        void onEvent(Event&);

        void pushLayer(Layer*);
        void pushOverlay(Layer*);

        [[nodiscard]] AppInfo getAppInfo() const;

        inline static Application& Get() { return *s_Instance; }
        inline Window& getWindow() { return *m_Window; }
    private:
        static Application* s_Instance;

        bool m_Running = true;

        std::unique_ptr<Window> m_Window;
        ImGuiLayer *m_ImGuiLayer;
        LayerStack m_LayerStack;
        float m_LastFrameTime = 0.0f;

        bool onWindowClose(WindowCloseEvent&);
        bool onWindowResize(WindowResizeEvent&);
    };

    Application* CreateApplication();
}
