#include <Stellar.h>

#include "imgui.h"

class ExampleLayer: public Stellar::Layer {
public: 
    ExampleLayer();

    void onUpdate(Stellar::Timestep ts) override;

    void onDetach() override;

    void onEvent(Stellar::Event& event) override;

    void onImGuiRender() override;

private:
    Stellar::Camera m_Camera{};
    glm::vec3 m_CameraPosition{0.0f, 2.0f, 0.5f};
    float m_CameraSpeed = 1.0f;

    Stellar::Buffer* m_VertexBuffer{};
    Stellar::Buffer* m_IndexBuffer{};
    Stellar::Texture2D* m_Texture{};

    const std::vector<Stellar::Vertex> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f}},
            {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}, {1.0f}},
            {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f}},
            {{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {1.0f}}
    };

    const std::vector<uint16_t> indices = {
            0, 1, 2, 2, 3, 0
    };

    glm::vec3 m_Color{1.0f};
};