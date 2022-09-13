#include <Stellar.h>

#include "imgui.h"

class ExampleLayer: public Stellar::Layer {
public: 
    ExampleLayer()
        : Layer("Example") {
        // vertex buffer
        auto vertexBufferSize = sizeof(vertices[0]) * vertices.size();
        auto* stagingBuffer = Stellar::Buffer::Create(Stellar::BufferType::Vertex,
                                                                vertexBufferSize,
                                                                vertices.data());
        m_VertexBuffer = Stellar::Buffer::Create(Stellar::BufferType::Vertex, vertexBufferSize);
        stagingBuffer->copy(*m_VertexBuffer);

        delete stagingBuffer;

        // index buffer
        auto indexBufferSize = sizeof(indices[0]) * indices.size();
        auto indexStagingBuffer = Stellar::Buffer::Create(Stellar::BufferType::Index,
                                                                    indexBufferSize,
                                                                    indices.data());
        m_IndexBuffer = Stellar::Buffer::Create(Stellar::BufferType::Index,  indexBufferSize);
        indexStagingBuffer->copy(*m_IndexBuffer);

        delete indexStagingBuffer;
    }

    void onUpdate() override {
        auto extent = Stellar::Application::Get().getWindow().getSwapChain()->getSwapChainExtent();
        m_Camera.setPerspectiveProjection(glm::radians(45.0f),
                                          (float)extent.width / (float) extent.height,
                                          0.1f, 10.0f);
        //m_Camera.setOrthographicProjection((float)extent.width, (float)extent.height, 0.1f, 10.0f);

        Stellar::Renderer::BeginScene(m_Camera);
        Stellar::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Stellar::Renderer::BeginRenderPass();
        Stellar::Renderer::RenderGeometry(m_VertexBuffer, m_IndexBuffer, indices.size());
        Stellar::Renderer::EndRenderPass();
        Stellar::Renderer::EndScene();
    }

    void onDetach() override {
        delete m_VertexBuffer;
        delete m_IndexBuffer;
    }

    void onEvent(Stellar::Event& event) override {

    }

    void onImGuiRender() override {
        auto appInfo = Stellar::Application::getAppInfo();

        ImGui::Begin("Info");

        ImGui::Text("GPU: %s", appInfo.graphicsInfo.c_str());
        ImGui::Text("Vulkan version: %s", appInfo.vulkanVersion.c_str());

        ImGui::Text(
                "Frame time: %.3f ms, FPS: %.1f FPS",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
        ImGui::End();
    }

private:
    Stellar::Camera m_Camera{};

    Stellar::Buffer* m_VertexBuffer{};
    Stellar::Buffer* m_IndexBuffer{};

    const std::vector<Stellar::Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
    };
};