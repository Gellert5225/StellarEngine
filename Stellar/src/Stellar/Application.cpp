#include "stlrpch.h"
#include "Application.h"

#include "Log.h"
#include "Input.h"

#include "Renderer/Uniforms.h"

namespace Stellar {
    Application* Application::s_Instance = nullptr;

    Application::Application() : m_Camera(-1.0f, -1.0f, -1.0f, -1.0f) {
        STLR_CORE_ASSERT(!s_Instance, "Application already exists")
        s_Instance = this;
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->init();
        m_Window->setEventCallback(BIND_EVENT_FN(Application::onEvent));

        Renderer::Init();
        m_CommandBuffer = CommandBuffer::Create(3);
        m_ImGuiLayer = new ImGuiLayer();
    }

    Application::~Application() {
        STLR_CORE_INFO("Shutting down");

        for (Layer* layer : m_LayerStack) {
            layer->onDetach();
            delete layer;
        }

        delete m_CommandBuffer;

        delete m_VertexBuffer;
        delete m_IndexBuffer;
        Renderer::Shutdown();

        m_Window = nullptr;
    };

    void Application::onEvent(Event& e) {
        EventDispatcher dispatcher(e);
        dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::onWindowClose));
        dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::onWindowResize));

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
        // vertex buffer
        auto vertexBufferSize = sizeof(vertices[0]) * vertices.size();
        auto* stagingBuffer = Buffer::Create(BufferType::Vertex, vertexBufferSize, vertices.data());
        m_VertexBuffer = Buffer::Create(BufferType::Vertex, vertexBufferSize);
        stagingBuffer->copy(*m_VertexBuffer);

        delete stagingBuffer;

        // index buffer
        auto indexBufferSize = sizeof(indices[0]) * indices.size();
        auto indexStagingBuffer = Buffer::Create(BufferType::Index, indexBufferSize, indices.data());
        m_IndexBuffer = Buffer::Create(BufferType::Index,  indexBufferSize);
        indexStagingBuffer->copy(*m_IndexBuffer);

        delete indexStagingBuffer;

        //m_UniformBuffer = Buffer::Create(BufferType::Uniform, sizeof(GlobalUniforms));

        while (m_Running) {
            for (Layer* layer : m_LayerStack)
                layer->onUpdate();
            m_Window->onUpdate();

            auto swapChain = m_Window->getSwapChain();
            swapChain->beginFrame();

            // geomoetry
            Renderer::BeginScene();
            m_CommandBuffer->begin();
            Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
            Renderer::BeginRenderPass(m_CommandBuffer);
            Renderer::RenderGeometry(m_CommandBuffer, m_VertexBuffer, m_IndexBuffer, indices.size());
            Renderer::EndRenderPass(m_CommandBuffer);
            m_CommandBuffer->end();
            m_CommandBuffer->submit();

            // imGui
            m_ImGuiLayer->begin();
            for (Layer* layer : m_LayerStack)
                layer->onImGuiRender();
            m_ImGuiLayer->end(m_CommandBuffer);

            // present
            m_Window->swapBuffers();
        }

        vkDeviceWaitIdle(VulkanDevice::GetInstance()->logicalDevice());
    }

    bool Application::onWindowClose(WindowCloseEvent& e) {
        m_Running = false;
        return true;
    }

    Application::AppInfo Application::getAppInfo() {
        return {
            "Stellar Engine",
            VulkanDevice::GetInstance()->getDeviceProperties().deviceName,
            VulkanInstance::GetInstance()->getInstanceVersion()
        };
    }

    bool Application::onWindowResize(WindowResizeEvent& e) {
        const uint32_t width = e.getWidth(), height = e.getHeight();
        if (width == 0 || height == 0) {
            //m_Minimized = true;
            return false;
        }
        //m_Minimized = false;

        m_Window->getSwapChain()->onResize();

        return false;
    }
}