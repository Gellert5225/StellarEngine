#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"

#include "Stellar/ImGUI/ImGuiLayer.h"

#include "Platform/Vulkan/Instance/VulkanInstance.h"
#include "Platform/Vulkan/Device/VulkanDevice.h"
#include "Platform/Vulkan/SwapChain/SwapChain.h"
#include "Platform/Vulkan/Pipeline/GraphicsPipeline.h"
#include "Platform/Vulkan/RenderPass/StandardRenderPass.h"
#include "Platform/Vulkan/Buffer/FrameBuffer.h"
#include "Platform/Vulkan/Command/CommandBuffer.h"
#include "Platform/Vulkan/Buffer/Buffer.h"
#include "Platform/Vulkan/Renderer/VulkanRendererContext.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"
#include "Renderer/Renderer.h"

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
        Buffer* m_VertexBuffer{};
        Buffer* m_IndexBuffer{};

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
