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

    glm::vec3 vertices[3] = {
        { -0.8f,  0.8f, 0.0f },
        {  0.0f, -0.8f, 0.0f },
        { +0.8f,  0.8f, 0.0f }
    };

    glm::vec3 colors[3] = {
        {  1.0, 0.3f, 0.2f },
        {  0.8f, 1.0, 0.0f },
        {  0.8f, 0.0f, 1.0 }
    };

    glm::vec3 m_Color{1.0f};

    Stellar::Buffer* m_VertexBuffer{};
    Stellar::Buffer* m_ColorBuffer{};
};
