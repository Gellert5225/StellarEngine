#pragma once

#include <Stellar/Core/Application.h>
#include <Stellar/Core/Log.h>
#include <Stellar/Scene/Scene.h>
#include <Stellar/Scene/Entity.h>
#include <Stellar/Scene/Components.h>
#include <Stellar/Events/KeyEvent.h>
#include <Stellar/Events/MouseEvent.h>

#include "Panels/SceneHierarchyPanel.h"

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
		bool onKeyPressed(KeyPressedEvent& e);
		bool onMouseButtonPressed(MouseButtonPressedEvent& e);

		void newScene();
		void openScene();
		void saveSceneAs();

		void menuBar();
	private:
		EditorCamera m_EditorCamera;
		SceneCamera m_SceneCamera;

		STLR_Ptr<Scene> m_ActiveScene;
		Entity m_LogoEntity;
		Entity m_ExampleEntity;
		Entity m_QuadEntity;
		Entity m_CameraEntity;

		ImVec2 m_ViewPortSize{ 1.0f, 1.0f };
		glm::vec2 m_ViewportBounds[2];

		SceneHierarchyPanel m_SceneHierarchyPanel;

		int m_GizmoType = -1;
		bool m_AllowViewportCameraEvents = false;
		bool m_ViewportPanelFocused = false;
		bool m_StartedRightClickInViewport = false;
		bool m_ViewportPanelMouseOver = false;
	};
}