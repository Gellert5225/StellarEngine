#include "SceneHierarchyPanel.h"

#include "Stellar/ImGui/ImGuiLayer.h"

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

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
			drawEntityNode(entity);
		});

		ImGui::End();

		ImGui::Begin("Properties");

		if (m_SelectionContext) {
			drawComponent(m_SelectionContext);
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::drawEntityNode(Entity entity) {
		auto& tag = entity.getComponent<TagComponent>().tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected  : 0 )| ImGuiTreeNodeFlags_OpenOnArrow;

		bool opened = ImGui::TreeNodeEx((void*)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked()) {
			m_SelectionContext = entity;
		}

		if (opened) {
			ImGui::TreePop();
		}
	}

	void SceneHierarchyPanel::drawComponent(Entity entity) {
		if (entity.hasComponent<TagComponent>()) {
			auto& tag = entity.getComponent<TagComponent>().tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());

			if (ImGui::InputText("Tag", buffer, sizeof(buffer))){
				tag = std::string(buffer);
			}
		}

		if (entity.hasComponent<TransformComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform")) {
				auto& transform = entity.getComponent<TransformComponent>().translation;
				ImGui::DragFloat3("Position", glm::value_ptr(transform), 0.1f); 
				ImGui::TreePop();
			}
		}

		if (entity.hasComponent<SpriteRendererComponent>()) {
			if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite")) {
				auto& sprite = entity.getComponent<SpriteRendererComponent>();
				ImGui::ColorEdit4("Color", glm::value_ptr(sprite.color));
				auto windowWidth = ImGui::GetContentRegionAvail();
				UI::Image(sprite.texture.get(), { windowWidth.x, windowWidth.x });

				ImGui::TreePop();
			}
		}
	}
}