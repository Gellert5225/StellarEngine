#include "ExampleLayer.h"

ExampleLayer::ExampleLayer() : Layer("Example") {
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

void ExampleLayer::onUpdate() {
    auto extent = Stellar::Application::Get().getWindow().getSwapChain()->getSwapChainExtent();
    m_Camera.setPerspectiveProjection(glm::radians(45.0f),
                                      (float)extent.width / (float) extent.height,
                                      0.1f, 10.0f);
    m_Camera.setPosition(m_CameraPosition);
    //m_Camera.setOrthographicProjection((float)extent.width, (float)extent.height, 0.1f, 10.0f);
    Stellar::Renderer::BeginScene(m_Camera);
    Stellar::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    Stellar::Renderer::BeginRenderPass();
    Stellar::Renderer::RenderGeometry(m_VertexBuffer, m_IndexBuffer, indices.size());
    Stellar::Renderer::EndRenderPass();
    Stellar::Renderer::EndScene();
}

void ExampleLayer::onDetach() {
    delete m_VertexBuffer;
    delete m_IndexBuffer;
}

void ExampleLayer::onEvent(Stellar::Event &event) {
    Stellar::EventDispatcher dispatcher(event);
    dispatcher.dispatch<Stellar::KeyPressedEvent>(BIND_EVENT_FN(ExampleLayer::onKeyPressedEvent));
}

bool ExampleLayer::onKeyPressedEvent(Stellar::KeyPressedEvent &event) {
    if (event.getKeyCode() == STLR_KEY_LEFT)
        m_CameraPosition.x -= m_CameraSpeed;
    if (event.getKeyCode() == STLR_KEY_RIGHT)
        m_CameraPosition.x += m_CameraSpeed;
    if (event.getKeyCode() == STLR_KEY_UP)
        m_CameraPosition.z += m_CameraSpeed;
    if (event.getKeyCode() == STLR_KEY_DOWN)
        m_CameraPosition.z -= m_CameraSpeed;
    return false;
}

void ExampleLayer::onImGuiRender() {
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
