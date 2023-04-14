#pragma once

#include <Stellar/Scene/Scene.h>
#include <Stellar/Scene/Components.h>
#include <Stellar/Scene/Entity.h>

namespace Stellar {
	class SceneHierarchyPanel {
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const STLR_Ptr<Scene>& scene);
		
		void setContext(const STLR_Ptr<Scene>& scene);

		void onImGuiRender();

		Entity getSelectedEntity() const { return m_SelectionContext; }
	private:
		void drawEntityNode(Entity entity);
		void drawComponent(Entity entity);
	private:
		STLR_Ptr<Scene> m_Context;
		Entity m_SelectionContext;
	};
}