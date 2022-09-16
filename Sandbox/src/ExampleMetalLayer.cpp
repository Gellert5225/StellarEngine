
#include "ExampleMetalLayer.h"
#include "imgui.h"

ExampleMetalLayer::ExampleMetalLayer() {

}

void ExampleMetalLayer::onUpdate(Stellar::Timestep ts) {
    Stellar::Renderer::BeginScene(m_Camera);
    Stellar::Renderer::SetClearColor({ 0.66f, 0.9f, 0.96f, 1.0f });
    Stellar::Renderer::BeginRenderPass();
    //Stellar::Renderer::RenderGeometry(m_VertexBuffer, m_IndexBuffer, indices.size(), transform);
    Stellar::Renderer::EndRenderPass();
    Stellar::Renderer::EndScene();
}

void ExampleMetalLayer::onDetach() {

}

void ExampleMetalLayer::onEvent(Stellar::Event &event) {
}

void ExampleMetalLayer::onImGuiRender() {
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
