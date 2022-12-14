#include "EditorLayer.h"
#include <imgui_internal.h>

namespace Stellar {
    EditorLayer::EditorLayer() : Layer("Sandbox2D") {
        m_Texture = Texture2D::Create("../Resources/Textures/Hermanos.png");
        m_Texture2 = Texture2D::Create("../Resources/Textures/Example_texture.jpg");
    }

    void EditorLayer::onAttach() {
        Renderer::SetClearColor({ 0.66f, 0.9f, 0.96f, 1.0f });

        m_ActiveScene = CreateRef<Scene>();
        m_LogoEntity = m_ActiveScene->createEntity("Logo Square");
        m_LogoEntity.addComponent<SpriteRendererComponent>(m_LogoColor, m_Texture);
        m_CameraEntity = m_ActiveScene->createEntity("Camera");
        m_CameraEntity.addComponent<CameraComponent>(m_Camera);
    }

    void EditorLayer::onDetach() {
        delete m_Texture;
        delete m_Texture2;
    }

    void EditorLayer::onUpdate(Timestep ts) {
        auto perspective = (float)m_ViewPortSize.x / (float)m_ViewPortSize.y;
        //m_Camera.setOrtho(-perspective, perspective, -1, 1, -10, 10);
        m_Camera.setPerspectiveProjection(glm::radians(60.0f), perspective, 0.1f, 100.0f);
        // camera movement
        if (Input::IsKeyPressed(STLR_KEY_W))
            m_CameraPosition += m_Camera.getFront() * m_CameraSpeed * float(ts);
        else if (Input::IsKeyPressed(STLR_KEY_S))
            m_CameraPosition -= m_Camera.getFront() * m_CameraSpeed * float(ts);
        if (Input::IsKeyPressed(STLR_KEY_A))
            m_CameraPosition -= m_Camera.getRight() * m_CameraSpeed * float(ts);
        else if (Input::IsKeyPressed(STLR_KEY_D))
            m_CameraPosition += m_Camera.getRight() * m_CameraSpeed * float(ts);
        if (Input::IsKeyPressed(STLR_KEY_SPACE))
            m_CameraPosition.y -= m_CameraSpeed * ts;
        else if (Input::IsKeyPressed(STLR_KEY_LEFT_CONTROL))
            m_CameraPosition.y += m_CameraSpeed * ts;
        if (Input::IsKeyPressed(STLR_KEY_LEFT))
            m_Camera.setYaw(-m_CameraSpeed * ts * 40);
        else if (Input::IsKeyPressed(STLR_KEY_RIGHT))
            m_Camera.setYaw(m_CameraSpeed * ts * 40);
        if (Input::IsKeyPressed(STLR_KEY_UP))
            m_Camera.setPitch(-m_CameraSpeed * ts * 40);
        else if (Input::IsKeyPressed(STLR_KEY_DOWN))
            m_Camera.setPitch(m_CameraSpeed * ts * 40);

        m_Camera.setPosition(m_CameraPosition);

        float angle = Timestep::GetTime()* glm::radians(90.0f);
        glm::mat4 transform = glm::translate(glm::mat4(1.f), glm::vec3(0.0f, -1.0f, 1.0f)) * 
                              glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1.0f, 0.0f, 0.0f)) *
                              glm::scale(glm::mat4(1.0f), glm::vec3(0.5f));
        
        auto& camera = m_CameraEntity.getComponent<CameraComponent>().camera;
        auto& squareTransform = m_LogoEntity.getComponent<TransformComponent>().transform;
        auto& squareColor = m_LogoEntity.getComponent<SpriteRendererComponent>().color;
        squareTransform = transform;
        squareColor = m_LogoColor;
        camera = m_Camera;
        m_ActiveScene->onUpdate(ts);
    }

    void EditorLayer::onEvent(Event& event) {

    }

    void EditorLayer::onImGuiRender() {
        auto appInfo = Application::Get().getAppInfo();

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiWindowFlags host_window_flags = 0;
        host_window_flags |= ImGuiWindowFlags_NoTitleBar | 
                            ImGuiWindowFlags_NoCollapse | 
                            ImGuiWindowFlags_NoResize | 
                            ImGuiWindowFlags_NoMove;
        host_window_flags |= ImGuiWindowFlags_MenuBar | 
                            ImGuiWindowFlags_NoBringToFrontOnFocus | 
                            ImGuiWindowFlags_NoNavFocus;
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            host_window_flags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(255,255,255,0));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(255,255,255,0));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(255,255,255,0));

        ImGui::Begin("DockSpace Window", nullptr, host_window_flags);
        ImGui::PopStyleVar(4);
        ImGui::PopStyleColor(3);

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) {
                    
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        
        ImGuiID dockspaceID = ImGui::GetID("HUB_DockSpace");
        ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspace_flags, nullptr);

        static auto first_time = true;
        if (first_time) {
            first_time = false;

            ImGui::DockBuilderRemoveNode(dockspaceID);
            ImGui::DockBuilderAddNode(dockspaceID, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

            auto dock_id_left = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.2f, nullptr, &dockspaceID);
            auto colorSetting1 = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.75f, nullptr, &dock_id_left);
            ImGui::DockBuilderDockWindow("Info", dock_id_left);
            ImGui::DockBuilderDockWindow("Color Setting", colorSetting1);
            ImGui::DockBuilderDockWindow("Color Setting 2", colorSetting1);
            ImGui::DockBuilderDockWindow("View Port", dockspaceID);
            ImGui::DockBuilderFinish(dockspaceID);
        }

        ImGui::End();

        //ImGui::SetNextWindowDockID(dockspaceID , ImGuiCond_FirstUseEver);
        ImGui::Begin("Info");
        ImGui::Text("GPU: %s", appInfo.graphicsInfo.c_str());
        ImGui::Text("%s", appInfo.vulkanVersion.c_str());
        ImGui::Text(
                "Frame time: %.3f ms, FPS: %.1f FPS",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
        ImGui::End();

        //ImGui::SetNextWindowDockID(dockspaceID , ImGuiCond_FirstUseEver);
        ImGui::Begin("Color Setting");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_Color));
        UI::Image(m_Texture2, { 200, 200 });
        ImGui::End();

        //ImGui::SetNextWindowDockID(dockspaceID , ImGuiCond_FirstUseEver);
        ImGui::Begin("Color Setting 2");
        auto& tag = m_LogoEntity.getComponent<TagComponent>().tag;
        ImGui::Text("%s", tag.c_str());
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_LogoColor));
        UI::Image(m_Texture, { 200, 200 });
        ImGui::End();

        // view port
        ImGuiIO& io = ImGui::GetIO();
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(255,255,255,0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("View Port" , nullptr, ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        m_ViewPortSize = ImGui::GetContentRegionAvail();
        Renderer::ResizeFrameBuffer(m_ViewPortSize.x, m_ViewPortSize.y);
        UI::ImageFromFB(Renderer::GetFrameBuffer(), m_ViewPortSize);

        ImGui::End();
    }
}