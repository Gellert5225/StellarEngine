#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"

#include "Stellar/ImGUI/ImGuiLayer.h"

#include "Render/Vulkan/Instance/VulkanInstance.h"
#include "Render/Vulkan/Device/VulkanDevice.h"
#include "Render/Vulkan/SwapChain/SwapChain.h"
#include "Render/Vulkan/Pipeline/GraphicsPipeline.h"
#include "Render/Vulkan/RenderPass/StandardRenderPass.h"
#include "Render/Vulkan/Buffer/FrameBuffer.h"
#include "Render/Vulkan/Command/CommandBuffer.h"
#include "Render/Vulkan/Buffer/Buffer.h"
#include "Render/Vulkan/Renderer/VulkanRendererContext.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

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

        [[nodiscard]] VulkanRendererContext* getRendererContext() const;

        inline static Application& Get() { return *s_Instance; }
        inline Window& getWindow() { return *m_Window; }
    private:
        static Application* s_Instance;

        bool m_Running = true;

        std::unique_ptr<Window> m_Window;
        std::unique_ptr<ImGuiLayer> m_ImGuiLayer;

        LayerStack m_LayerStack;
        Buffer* m_VertexBuffer{};
        VulkanRendererContext* m_RenderContext;

        bool onWindowClose(WindowCloseEvent&);

        const std::vector<Vertex> vertices = {
                {{ 0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
                {{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
                {{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}
        };
    };

    Application* CreateApplication();
}
