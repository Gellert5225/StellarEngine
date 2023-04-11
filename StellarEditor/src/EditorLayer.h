#pragma once

#include <Stellar/Core/Application.h>
#include <Stellar/Core/Log.h>
#include <Stellar/Scene/Scene.h>
#include <Stellar/Scene/Entity.h>
#include <Stellar/Scene/Components.h>
#include <Stellar/Events/KeyEvent.h>

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

		void newScene();
		void openScene();
		void saveSceneAs();

		void menuBar();
	private:
		EditorCamera m_EditorCamera;
		SceneCamera m_SceneCamera;

		Ref<Scene> m_ActiveScene;
		Entity m_LogoEntity;
		Entity m_ExampleEntity;
		Entity m_QuadEntity;
		Entity m_CameraEntity;

		ImVec2 m_ViewPortSize{ 1.0f, 1.0f };

		SceneHierarchyPanel m_SceneHierarchyPanel;

		int m_GizmoType = -1;
		bool m_ViewportFocused = false, m_ViewportHovered = false;
	};
}