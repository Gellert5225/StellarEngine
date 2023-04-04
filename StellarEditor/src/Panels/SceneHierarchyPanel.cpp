#include "SceneHierarchyPanel.h"

#include <imgui.h>

namespace Stellar {
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene) {
		setContext(scene);
	}
	
	void SceneHierarchyPanel::setContext(const Ref<Scene>& scene) {
		m_Context = scene;
	}

	void SceneHierarchyPanel::onImGuiRender() {
		ImGui::Begin("Scene Hierarchy");
		
		m_Context->m_Registry.each([&](auto entityID) {
			Entity entity{ entityID, m_Context.get() };
			auto& tc = entity.getComponent<TagComponent>();
			ImGui::Text("%s", tc.tag.c_str());
		});

		ImGui::End();
	}
}