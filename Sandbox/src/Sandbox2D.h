#pragma once

#include <Stellar.h>

#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"

class Sandbox2D : public Stellar::Layer {
public:
    void onAttach() override;
    void onDetach() override;
    void onUpdate(Stellar::Timestep ts) override;
    void onEvent(Stellar::Event& event) override;
    void onImGuiRender() override;
private:
    Stellar::Camera m_Camera{};
    glm::vec3 m_CameraPosition{0.0f, 2.0f, 0.5f};
    float m_CameraSpeed = 1.0f;
    
    Stellar::Texture2D* m_Texture{};

    glm::vec3 m_Color{1.0f};
};