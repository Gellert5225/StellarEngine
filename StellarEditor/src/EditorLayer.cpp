#include "EditorLayer.h"
#include <imgui_internal.h>
#include <ImGuizmo.h>

#include <Stellar/Scene/SceneSerializer.h>
#include <Stellar/Utils/FileUtil.h>
#include <Stellar/Events/Event.h>
#include <Stellar/Core/KeyCodes.h>
#include <Stellar/Core/Input.h>
#include <Stellar/Maths/Math.h>

namespace Stellar {
	EditorLayer::EditorLayer() : Layer("Sandbox2D"), m_EditorCamera(60.0f, 1.0f, 1.0f, 0.1f, 1000.0f) {
		m_SceneCamera.setPerspective(60.0f, 0.1f, 1000.0f);
	}

	void EditorLayer::onAttach() {
		Renderer::SetClearColor({ 0.66f, 0.9f, 0.96f, 1.0f });

		m_ActiveScene = STLR_Ptr<Scene>::Create();
		m_ExampleEntity = m_ActiveScene->createEntity("Example Square");
		m_ExampleEntity.addComponent<SpriteRendererComponent>(glm::vec4{1.0f}, Texture2D::Create("Resources/Textures/Example_texture.jpg"));

		m_LogoEntity = m_ActiveScene->createEntity("Logo Square"); 
		m_LogoEntity.addComponent<SpriteRendererComponent>(glm::vec4{1.0f}, Texture2D::Create("Resources/Textures/StellarEngine.png"));
		
		m_QuadEntity = m_ActiveScene->createEntity("Quad Square");
		m_QuadEntity.addComponent<SpriteRendererComponent>(glm::vec4{1.0f}, Texture2D::Create(ImageFormat::RGBA));

		m_CameraEntity = m_ActiveScene->createEntity("Scene Camera");
		m_CameraEntity.addComponent<CameraComponent>(m_SceneCamera);

		m_EditorCamera = EditorCamera(60.0f, (float)m_ViewPortSize.x, (float)m_ViewPortSize.y, 0.1f, 100.0f);

		m_SceneHierarchyPanel.setContext(m_ActiveScene);
	}

	void EditorLayer::onDetach() {}

	void EditorLayer::onUpdate(Timestep ts) {
		m_EditorCamera.setActive(m_AllowViewportCameraEvents);
		m_EditorCamera.onUpdate(ts);
		
		if (Input::IsMouseButtonPressed(STLR_MOUSE_RIGHT) && !m_StartedRightClickInViewport && m_ViewportPanelFocused && m_ViewportPanelMouseOver)
			m_StartedRightClickInViewport = true;

		if (!Input::IsMouseButtonPressed(STLR_MOUSE_RIGHT))
			m_StartedRightClickInViewport = false;

		m_ActiveScene->onEditorUpdate(ts, m_EditorCamera);
	}

	void EditorLayer::onEvent(Event& event) {
		// if (m_AllowViewportCameraEvents)
		// 	m_EditorCamera.onEvent(event);
		EventDispatcher dispatcher(event);
		dispatcher.dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::onKeyPressed));
		dispatcher.dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::onMouseButtonPressed));
	}

	void EditorLayer::onImGuiRender() {
		auto appInfo = Application::Get().getAppInfo();

		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || 
			(ImGui::IsMouseClicked(ImGuiMouseButton_Right) && !m_StartedRightClickInViewport)) {
			ImGui::FocusWindow(GImGui->HoveredWindow);
			Input::SetCursorMode(CursorMode::Normal);
		}

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::SetKeyOwner(ImGuiMod_Alt, viewport->ID);

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

		ImGui::Begin("DockSpace Window", nullptr, host_window_flags);
		ImGui::PopStyleVar(4);

		menuBar();

		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSize = style.WindowMinSize.x;
		style.WindowMinSize.x = 320.0f;

		ImGuiID dockspaceID = ImGui::GetID("HUB_DockSpace");
		ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), dockspace_flags, nullptr);

		style.WindowMinSize.x = minWinSize;

		static auto first_time = true;
		if (first_time) {
			first_time = false;

			ImGui::DockBuilderRemoveNode(dockspaceID);
			ImGui::DockBuilderAddNode(dockspaceID, dockspace_flags | ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->Size);

			auto dock_id_right = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Right, 0.3f, nullptr, &dockspaceID);
			auto dock_id_left = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Left, 0.3f, nullptr, &dockspaceID);
			auto properties = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.75f, nullptr, &dock_id_left);
			auto log = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Down, 0.25f, nullptr, &dockspaceID);
			ImGui::DockBuilderDockWindow("Info", dock_id_right);
			ImGui::DockBuilderDockWindow("Scene Hierarchy", dock_id_left);
			ImGui::DockBuilderDockWindow("Properties", properties);
			ImGui::DockBuilderDockWindow("View Port", dockspaceID);
			ImGui::DockBuilderDockWindow("Log", log);
			ImGui::DockBuilderFinish(dockspaceID);
		}

		m_SceneHierarchyPanel.onImGuiRender();
		static bool isOpen = true;
		m_ConsolePanel.onImGuiRender(isOpen);
		m_ResourcePanel.onImGuiRender(isOpen);

		//ImGui::SetNextWindowDockID(dockspaceID , ImGuiCond_FirstUseEver);
		ImGui::Begin("Info");
		std::string debug;
#ifdef NDEBUG
		debug = "Release";
#else
		debug = "Debug";
#endif
		ImGui::Text("Version: %s", debug.c_str());
		ImGui::Text("GPU: %s", appInfo.graphicsInfo.c_str());
		ImGui::Text("%s", appInfo.vulkanVersion.c_str());
		ImGui::Text(
				"Frame time: %.3f ms, FPS: %.1f FPS",
				1000.0f / ImGui::GetIO().Framerate,
				ImGui::GetIO().Framerate);

		ImGui::Text("Quad Count: %i", m_ActiveScene->getRenderer2DStats().quadCount);
		ImGui::Text("Draw Calls: %i", m_ActiveScene->getRenderer2DStats().drawCalls);
		ImGui::Text("Vertices: %i", m_ActiveScene->getRenderer2DStats().getTotalVertexCount());
		ImGui::Text("Indicies: %i", m_ActiveScene->getRenderer2DStats().getTotalIndexCount());
		ImGui::End();

		// view port
		ImGuiIO& io = ImGui::GetIO();
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(255,255,255,0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("View Port");

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		io.ConfigWindowsMoveFromTitleBarOnly = true;
		m_ViewPortSize = ImGui::GetContentRegionAvail();
		m_EditorCamera.setViewportSize(m_ViewPortSize.x, m_ViewPortSize.y);
		Renderer::ResizeFrameBuffer(m_ViewPortSize.x, m_ViewPortSize.y);
		UI::ImageFromFB(Renderer::GetFrameBuffer(), m_ViewPortSize);

		m_ViewportPanelMouseOver = ImGui::IsWindowHovered();
		m_ViewportPanelFocused = ImGui::IsWindowFocused();
		
		m_AllowViewportCameraEvents = m_ViewportPanelFocused || m_StartedRightClickInViewport;

		// gizmo
		Entity selected = m_SceneHierarchyPanel.getSelectedEntity();
		if (selected && m_GizmoType != -1) {
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetGizmoSizeClipSpace(0.2f);

			float windowWidth = (float)ImGui::GetWindowWidth();
			float windowHeight = (float)ImGui::GetWindowHeight();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);

			// Editor camera
			const glm::mat4& cameraProjection = m_EditorCamera.getProjectionMatrix();
			glm::mat4 cameraView = m_EditorCamera.getViewmatrix();

			// Entity transform
			auto& tc = selected.getComponent<TransformComponent>();
			glm::mat4 transform = tc.getTransform();

			// Snapping
			bool snap = Input::IsKeyPressed(STLR_KEY_LEFT_CONTROL);
			float snapValue = 0.5f; // Snap to 0.5m for translation/scale
			// Snap to 45 degrees for rotation
			if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
				snapValue = 45.0f;

			float snapValues[3] = { snapValue, snapValue, snapValue };

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
				(ImGuizmo::OPERATION)m_GizmoType, ImGuizmo::LOCAL, glm::value_ptr(transform),
				nullptr, snap ? snapValues : nullptr);

			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;
				Math::DecomposeTransform(transform, translation, rotation, scale);

				glm::vec3 deltaRotation = rotation - tc.rotation;
				tc.translation = translation;
				tc.rotation += deltaRotation;
				tc.scale = scale;
			}
		}

		ImGui::End();

		ImGui::End();
	}

	bool EditorLayer::onKeyPressed(KeyPressedEvent& e) {
		if (e.getRepeatCount() > 0) return false;

		bool control = Input::IsKeyPressed(STLR_KEY_LEFT_CONTROL) || Input::IsKeyPressed(STLR_KEY_RIGHT_CONTROL);
		bool shift = Input::IsKeyPressed(STLR_KEY_LEFT_SHIFT) || Input::IsKeyPressed(STLR_KEY_RIGHT_SHIFT);
		switch (e.getKeyCode()) {
			case STLR_KEY_N:
				if (control)
					newScene();
				break;
			case STLR_KEY_O:
				if (control)
					openScene();
				break;
			case STLR_KEY_S:
				if (control && shift)
					saveSceneAs();
				break;
			case STLR_KEY_Q:
				if (!ImGuizmo::IsUsing())
					m_GizmoType = -1;
				break;
			case STLR_KEY_W:
				if (!ImGuizmo::IsUsing())
					m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				break;
			case STLR_KEY_E:
				if (!ImGuizmo::IsUsing())
					m_GizmoType = ImGuizmo::OPERATION::ROTATE;
				break;
			case STLR_KEY_R:
				if (!ImGuizmo::IsUsing())
					m_GizmoType = ImGuizmo::OPERATION::SCALE;
				break;
		}
		return false;
	}

	bool EditorLayer::onMouseButtonPressed(MouseButtonPressedEvent& e) {
		if (e.getMouseButton() != STLR_MOUSE_LEFT)
			return false;
		if (!m_ViewportPanelMouseOver)
			return false;
		if (Input::IsKeyPressed(STLR_KEY_LEFT_ALT) || Input::IsMouseButtonPressed(STLR_MOUSE_RIGHT))
			return false;

		return false;
	}

	void EditorLayer::newScene() {
		m_ActiveScene = STLR_Ptr<Scene>::Create();
		m_SceneHierarchyPanel.setContext(m_ActiveScene);
	}

	void EditorLayer::openScene() {
		std::string filePath = FileDialogs::OpenFile("stlr");
		if (!filePath.empty()) {
			m_ActiveScene = STLR_Ptr<Scene>::Create();
			m_SceneHierarchyPanel.setContext(m_ActiveScene);

			SceneSerializer serializer(m_ActiveScene);
			serializer.deserialize(filePath);
		}
	}

	void EditorLayer::saveSceneAs() {
		std::string filePath = FileDialogs::SaveFile("stlr");
		if (!filePath.empty()) {
			SceneSerializer serializer(m_ActiveScene);
			serializer.serialize(filePath);
		}
	}

	void EditorLayer::menuBar() {
		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("New", "Ctrl+N")) newScene();
				if (ImGui::MenuItem("Open", "Ctrl+O")) openScene();
				if (ImGui::MenuItem("Save As", "Ctrl+Shift+S")) saveSceneAs();
				if (ImGui::MenuItem("Exit")) Application::Get().close();
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
	}
}