#pragma once

#include <Stellar/Scene/Scene.h>
#include <Stellar/Scene/Components.h>
#include <Stellar/Scene/Entity.h>

namespace Stellar {
	class SceneHierarchyPanel {
	public:
		SceneHierarchyPanel() = default;
		SceneHierarchyPanel(const Ref<Scene>& scene);
		
		void setContext(const Ref<Scene>& scene);

		void onImGuiRender();
	private:
		Ref<Scene> m_Context;
	};
}