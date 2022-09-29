#include "Sandbox2D.h"

void Sandbox2D::onAttach() {

}

void Sandbox2D::onDetach() {

}

void Sandbox2D::onUpdate(Stellar::Timestep ts) {
    
}

void Sandbox2D::onEvent(Stellar::Event& event) {

}

void Sandbox2D::onImGuiRender() {
    auto appInfo = Stellar::Application::Get().getAppInfo();

    ImGui::Begin("Info");
    ImGui::Text("GPU: %s", appInfo.graphicsInfo.c_str());
    ImGui::Text("%s", appInfo.vulkanVersion.c_str());
    ImGui::Text(
            "Frame time: %.3f ms, FPS: %.1f FPS",
            1000.0f / ImGui::GetIO().Framerate,
            ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Begin("Color Setting");
    ImGui::ColorEdit3("Square Color", glm::value_ptr(m_Color));
    ImGui::End();
}