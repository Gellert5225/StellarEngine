#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"

#include "ImGUI/ImGuiLayer.h"

#include "Platform/Vulkan/Instance/VulkanInstance.h"
#include "Platform/Vulkan/Device/VulkanDevice.h"

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

        static AppInfo getAppInfo();

        //[[nodiscard]] VulkanRendererContext* getRendererContext() const;

        inline static Application& Get() { return *s_Instance; }
        inline Window& getWindow() { return *m_Window; }
    private:
        static Application* s_Instance;

        bool m_Running = true;

        std::unique_ptr<Window> m_Window;
        ImGuiLayer *m_ImGuiLayer;
        LayerStack m_LayerStack;
        Camera m_Camera{};

        Buffer* m_VertexBuffer{};
        Buffer* m_IndexBuffer{};
        Buffer* m_UniformBuffer{};
        CommandBuffer* m_CommandBuffer{};

        bool onWindowClose(WindowCloseEvent&);
        bool onWindowResize(WindowResizeEvent&);

        const std::vector<Vertex> vertices = {
                {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
                {{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}
        };

        const std::vector<uint16_t> indices = {
                0, 1, 2, 2, 3, 0
        };
    };

    Application* CreateApplication();
}
