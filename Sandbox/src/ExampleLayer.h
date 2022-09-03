#include <Stellar.h>

#include "imgui.h"

class ExampleLayer: public Stellar::Layer {
public: 
    ExampleLayer()
        : Layer("Example") {}

    void onUpdate() override {
        // STLR_INFO("ExampleLayer::Update");
    }

    void onEvent(Stellar::Event& event) override {
        if (event.getEventType() == Stellar::EventType::KeyPressed) {
            auto& e = (Stellar::KeyPressedEvent&)(event);
            STLR_TRACE("{0}", e.getKeyCode());
        }
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
};