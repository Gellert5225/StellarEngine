#pragma once

#include <Stellar.h>

class ExampleMetalLayer : public Stellar::Layer {
public:
    ExampleMetalLayer();
    void onUpdate(Stellar::Timestep ts) override;
    void onAttach() override;
    void onDetach() override;
    void onEvent(Stellar::Event& event) override;
    void onImGuiRender() override;

private:
    Stellar::Camera m_Camera{};
    glm::vec3 m_CameraPosition{0.0f, 0.0f, -2.0f};
    float m_CameraSpeed = 1.0f;

    glm::vec3 m_Color{1.0f};
};
