#pragma once

#include <Stellar.h>

#include "imgui.h"
#include "glm/gtc/type_ptr.hpp"

namespace Stellar {
    class EditorLayer : public Layer {
    public:
        EditorLayer();
        void onAttach() override;
        void onDetach() override;
        void onUpdate(Timestep ts) override;
        void onEvent(Event& event) override;
        void onImGuiRender() override;
    private:
        Camera m_Camera{};
        glm::vec3 m_CameraPosition{0.0f, 1.0f, -2.0f};
        float m_CameraSpeed = 1.0f;
        
        Texture2D* m_Texture{};
        Texture2D* m_Texture2{};

        glm::vec3 m_Color{1.0f};
        glm::vec4 m_LogoColor{1.0f};

        Ref<Scene> m_ActiveScene;
        Entity m_LogoEntity;
        Entity m_CameraEntity;

        ImVec2 m_ViewPortSize{ 1.0f, 1.0f };
    };
}