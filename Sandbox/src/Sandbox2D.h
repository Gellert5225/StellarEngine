#pragma once

#include <Stellar.h>

#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"

class Sandbox2D : public Stellar::Layer {
public:
    Sandbox2D();
    void onAttach() override;
    void onDetach() override;
    void onUpdate(Stellar::Timestep ts) override;
    void onEvent(Stellar::Event& event) override;
    void onImGuiRender() override;
private:
    Stellar::Camera m_Camera{};
    glm::vec3 m_CameraPosition{0.0f, -1.0f, -2.0f};
    float m_CameraSpeed = 1.0f;
    
    std::shared_ptr<Stellar::Texture2D> m_Texture;
    // std::unique_ptr<Stellar::Texture2D> m_Texture2{};
    Stellar::Texture2D* m_Texture2{};

    glm::vec3 m_Color{1.0f};
};