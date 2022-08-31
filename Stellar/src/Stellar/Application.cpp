#include "stlrpch.h"
#include "Application.h"

#include "Log.h"

namespace Stellar {
    Application* Application::s_Instance = nullptr;

    Application::Application() {
        STLR_CORE_ASSERT(!s_Instance, "Application already exists")
        s_Instance = this;
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->setEventCallback(BIND_EVENT_FN(Application::onEvent));
        m_RenderContext = new VulkanRendererContext(*m_Window);

        m_ImGuiLayer = std::make_unique<ImGuiLayer>();
    }

    Application::~Application() = default;

    void Application::onEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));

        for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();) {
            (*--it)->onEvent(e);
            if (e.handled)
                break;
        }
    }

    void Application::pushLayer(Layer* layer) {
        m_LayerStack.pushLayer(layer);
        layer->onAttach();
    }

    void Application::pushOverlay(Layer* layer) {
        m_LayerStack.pushOverlay(layer);
        layer->onAttach();
    }

    void Application::run() {
        m_VertexBuffer = new VertexBuffer(&vertices);

        while (m_Running) {
            for (Layer* layer : m_LayerStack)
                layer->onUpdate();
            m_Window->onUpdate();

            if (VkCommandBuffer commandBuffer = m_RenderContext->beginFrame()) {
                m_RenderContext->beginRenderPass(commandBuffer);

                VkBuffer vertexBuffers[] = {m_VertexBuffer->getVertexBuffer()};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

                vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_VertexBuffer->getVertices()->size()), 1, 0, 0);

                m_ImGuiLayer->begin();
                for (Layer* layer : m_LayerStack)
                    layer->onImGuiRender();
                m_ImGuiLayer->end(commandBuffer);

                m_RenderContext->endRenderPass(commandBuffer);
                m_RenderContext->endFrame();
            }
        }

        vkDeviceWaitIdle(VulkanDevice::GetInstance()->logicalDevice());

        m_Window = nullptr;
    }

    bool Application::onWindowClose(WindowCloseEvent& e) {
        m_Running = false;
        return true;
    }

    VulkanRendererContext* Application::getRendererContext() const {
        return m_RenderContext;
    }

    Application::AppInfo Application::getAppInfo() {
        return {
            "Stellar Engine",
            VulkanDevice::GetInstance()->getDeviceProperties().deviceName,
            VulkanInstance::GetInstance()->getInstanceVersion()
        };
    }
}