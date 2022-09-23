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

    m_Texture = Stellar::Texture2D::Create("../Resources/Textures/StellarEngine.png");
}

void ExampleLayer::onUpdate(Stellar::Timestep ts) {
    auto extent = Stellar::Application::Get().getWindow().getSwapChain()->getSwapChainExtent();
    auto perspective = (float)extent.width / (float)extent.height;
    //m_Camera.setOrtho(-perspective, perspective, -1, 1, -10, 10);
    m_Camera.setPerspectiveProjection(glm::radians(60.0f), perspective, 0.1f, 10.0f);

    // camera movement
    if (Stellar::Input::IsKeyPressed(STLR_KEY_LEFT))
        m_CameraPosition.x += m_CameraSpeed * ts;
    else if (Stellar::Input::IsKeyPressed(STLR_KEY_RIGHT))
        m_CameraPosition.x -= m_CameraSpeed * ts;
    if (Stellar::Input::IsKeyPressed(STLR_KEY_UP))
        m_CameraPosition.z += m_CameraSpeed * ts;
    else if (Stellar::Input::IsKeyPressed(STLR_KEY_DOWN))
        m_CameraPosition.z -= m_CameraSpeed * ts;

    glm::mat4 transform = glm::translate(glm::mat4(1.f), glm::vec3(1.0f, 0.0f, 0.0f))
            * glm::rotate(glm::mat4(1.0f),
                          Stellar::Timestep::GetTime()* glm::radians(90.0f),
                          glm::vec3(1.0f, 0.0f, 0.0f));

    m_Camera.setPosition(m_CameraPosition);
    Stellar::Renderer::BeginScene(m_Camera);
    Stellar::Renderer::SetClearColor({ 0.66f, 0.9f, 0.96f, 1.0f });
    Stellar::Renderer::BeginRenderPass();
    m_Texture->bind();
    Stellar::Renderer::RenderGeometry(m_VertexBuffer, m_IndexBuffer, indices.size(), transform);
    Stellar::Renderer::EndRenderPass();
    Stellar::Renderer::EndScene();
}

void ExampleLayer::onDetach() {
    delete m_VertexBuffer;
    delete m_IndexBuffer;
}

void ExampleLayer::onEvent(Stellar::Event &event) {

}

void ExampleLayer::onImGuiRender() {
    auto appInfo = Stellar::Application::Get().getAppInfo();

    ImGui::Begin("Info");

    ImGui::Text("GPU: %s", appInfo.graphicsInfo.c_str());
    ImGui::Text("%s", appInfo.vulkanVersion.c_str());

    ImGui::Text(
            "Frame time: %.3f ms, FPS: %.1f FPS",
            1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);
    ImGui::End();
}
