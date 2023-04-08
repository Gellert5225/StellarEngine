#include "EditorLayer.h"
#include <imgui_internal.h>

namespace Stellar {
	EditorLayer::EditorLayer() : Layer("Sandbox2D"), m_EditorCamera(60.0f, 1.0f, 0.1f, 1000.0f) {
		m_SceneCamera.setPerspective(60.0f, 0.1f, 1000.0f);
	}

	void EditorLayer::onAttach() {
		Renderer::SetClearColor({ 0.66f, 0.9f, 0.96f, 1.0f });

		m_ActiveScene = CreateRef<Scene>();
		
		m_ExampleEntity = m_ActiveScene->createEntity("Example Square");
		m_ExampleEntity.addComponent<SpriteRendererComponent>(glm::vec4{1.0f}, Texture2D::Create("../Resources/Textures/Example_texture.jpg"));

		m_LogoEntity = m_ActiveScene->createEntity("Logo Square");
		m_LogoEntity.addComponent<SpriteRendererComponent>(glm::vec4{1.0f}, Texture2D::Create("../Resources/Textures/StellarEngine.png"));
		
		m_QuadEntity = m_ActiveScene->createEntity("Quad Square");
		m_QuadEntity.addComponent<SpriteRendererComponent>(glm::vec4{1.0f}, Texture2D::Create(ImageFormat::RGBA));

		m_CameraEntity = m_ActiveScene->createEntity("Scene Camera");
		m_CameraEntity.addComponent<CameraComponent>(m_SceneCamera);

		auto perspective = (float)m_ViewPortSize.x / (float)m_ViewPortSize.y;
		m_EditorCamera = EditorCamera(60.0f, perspective, 0.1f, 100.0f);

		m_SceneHierarchyPanel.setContext(m_ActiveScene);
	}

	void EditorLayer::onDetach() {}

	void EditorLayer::onUpdate(Timestep ts) {
		m_EditorCamera.SetViewportSize(m_ViewPortSize.x, m_ViewPortSize.y);
		m_EditorCamera.onUpdate(ts);
		
		//float angle = Timestep::GetTime()* glm::radians(90.0f);

		// auto& sceneCamera = m_CameraEntity.getComponent<CameraComponent>().camera;

		// sceneCamera.setViewPortSize(m_ViewPortSize.x, m_ViewPortSize.y);

		// m_LogoEntity.getComponent<TransformComponent>().rotation = glm::vec3(angle, 0.0f, 0.0f);
		// m_ExampleEntity.getComponent<TransformComponent>().rotation = glm::vec3(angle, 0.0f, 0.0f);
		//m_ActiveScene->onUpdate(ts);
		m_ActiveScene->onEditorUpdate(ts, m_EditorCamera);
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

			auto dock_id_right = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Right, 0.2f, nullptr, &dockspaceID);
			auto dock_id_left = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.2f, nullptr, &dockspaceID);
			auto properties = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.75f, nullptr, &dock_id_left);
			ImGui::DockBuilderDockWindow("Info", dock_id_right);
			ImGui::DockBuilderDockWindow("Scene Hierarchy", dock_id_left);
			ImGui::DockBuilderDockWindow("Properties", properties);
			ImGui::DockBuilderDockWindow("View Port", dockspaceID);
			ImGui::DockBuilderFinish(dockspaceID);
		}

		ImGui::End();

		m_SceneHierarchyPanel.onImGuiRender();
		//ImGui::SetNextWindowDockID(dockspaceID , ImGuiCond_FirstUseEver);
		ImGui::Begin("Info");
		ImGui::Text("GPU: %s", appInfo.graphicsInfo.c_str());
		ImGui::Text("%s", appInfo.vulkanVersion.c_str());
		ImGui::Text(
				"Frame time: %.3f ms, FPS: %.1f FPS",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);
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