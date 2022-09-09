#include "stlrpch.h"
#include "Application.h"

#include "Log.h"
#include "Input.h"

namespace Stellar {
    Application* Application::s_Instance = nullptr;

    Application::Application() {
        STLR_CORE_ASSERT(!s_Instance, "Application already exists")
        s_Instance = this;
        m_Window = std::unique_ptr<Window>(Window::Create());
        m_Window->init();
        m_Window->setEventCallback(BIND_EVENT_FN(Application::onEvent));

        m_ImGuiLayer = new ImGuiLayer();

        Renderer::Init();
    }

    Application::~Application() {
        STLR_CORE_INFO("Shutting down");

        for (Layer* layer : m_LayerStack) {
            layer->onDetach();
            delete layer;
        }

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
        auto* stagingBuffer = new Buffer(vertexBufferSize,
                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        vertices.data());
        m_VertexBuffer = new Buffer(vertexBufferSize,
                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        Buffer::CopyBuffer(*stagingBuffer, *m_VertexBuffer, vertexBufferSize);

        delete stagingBuffer;

        // index buffer
        auto indexBufferSize = sizeof(indices[0]) * indices.size();
        auto indexStagingBuffer =new Buffer(indexBufferSize,
                                            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                            indices.data());
        m_IndexBuffer = new Buffer(indexBufferSize,
                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                   VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        Buffer::CopyBuffer(*indexStagingBuffer, *m_IndexBuffer, indexBufferSize);

        delete indexStagingBuffer;

        while (m_Running) {
            for (Layer* layer : m_LayerStack)
                layer->onUpdate();
            m_Window->onUpdate();

            // begin frame
            // begin command buffer
            // begin render pass
            // draw call
            // end render pass
            // end command buffer
            // end frame

            auto swapChain = m_Window->getSwapChain();
            swapChain->beginFrame();
            auto commandBuffer = swapChain->getCurrentCommandBuffer();
            // begin commandbuffer
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 0.1f });
            Renderer::BeginRenderPass(commandBuffer);

            Renderer::RenderGeometry();

            VkBuffer vertexBuffers[] = {m_VertexBuffer->getBuffer()};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT16);

            vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);

            m_ImGuiLayer->begin();
            for (Layer* layer : m_LayerStack)
                layer->onImGuiRender();
            m_ImGuiLayer->end(commandBuffer);

            Renderer::EndRenderPass(commandBuffer);
            // end command buffer
            if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
                throw std::runtime_error("failed to record command buffer!");

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