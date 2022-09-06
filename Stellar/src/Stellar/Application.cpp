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
        // staging buffer
        auto stagingBuffer = new Buffer(sizeof(vertices[0]) * vertices.size(),
                                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        vertices.data());
        m_VertexBuffer = new Buffer(sizeof(vertices[0]) * vertices.size(),
                                    VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                    VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        VkCommandBuffer cb = VulkanDevice::GetInstance()->beginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;  // Optional
        copyRegion.dstOffset = 0;  // Optional
        copyRegion.size = sizeof(vertices[0]) * vertices.size();
        vkCmdCopyBuffer(cb, stagingBuffer->getBuffer(),
                        m_VertexBuffer->getBuffer(), 1, &copyRegion);

        VulkanDevice::GetInstance()->endSingleTimeCommands(cb);

        while (m_Running) {
            for (Layer* layer : m_LayerStack)
                layer->onUpdate();
            m_Window->onUpdate();

            if (VkCommandBuffer commandBuffer = m_RenderContext->beginFrame()) {
                m_RenderContext->beginRenderPass(commandBuffer);

                VkBuffer vertexBuffers[] = {m_VertexBuffer->getBuffer()};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

                vkCmdDraw(commandBuffer, static_cast<uint32_t>(vertices.size()), 1, 0, 0);

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