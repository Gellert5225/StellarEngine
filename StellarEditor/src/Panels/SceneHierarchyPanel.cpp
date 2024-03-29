#include "SceneHierarchyPanel.h"

#include "Stellar/ImGui/ImGuiLayer.h"
#include "Stellar/ImGui/WebFont.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>

namespace Stellar {
	SceneHierarchyPanel::SceneHierarchyPanel(const STLR_Ptr<Scene>& scene) {
		setContext(scene);
	}
	
	void SceneHierarchyPanel::setContext(const STLR_Ptr<Scene>& scene) {
		m_Context = scene;
		m_SelectionContext = {};
	}

	void SceneHierarchyPanel::onImGuiRender() {
		ImGui::Begin("Scene Hierarchy");
		
		m_Context->m_Registry.each([&](auto entityID) {
			Entity entity{ entityID, m_Context.raw() };
			drawEntityNode(entity);
		});

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectionContext = {};

		if (ImGui::BeginPopupContextWindow(0, 1 | ImGuiPopupFlags_NoOpenOverItems)) {
			if (ImGui::MenuItem("Create Empty Entity")) {
				m_Context->createEntity("Empty Entity");
			}

			ImGui::EndPopup();
		}

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
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		bool opened = ImGui::TreeNodeEx((void*)(size_t)(uint32_t)entity, flags, "%s", tag.c_str());
		if (ImGui::IsItemClicked()) {
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem()) {
			if (ImGui::MenuItem("Delete Entity"))
				entityDeleted = true;

			ImGui::EndPopup();
		}

		if (opened) {
			ImGui::TreePop();
		}

		if (entityDeleted) {
			m_Context->destroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
	}

	static void DrawVec3Control(const std::string& label, glm::vec3& value, float resetVal = 0.0f, float columnWidth = 100.0f) {
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];
		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text("%s", label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			value.x = resetVal;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);

		ImGui::SameLine();
		ImGui::DragFloat("##X", &value.x, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.3f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			value.y = resetVal;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &value.y, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();
		
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f});
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f});
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			value.z = resetVal;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &value.z, 0.1f, 0.0f, 0.0f, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		
		ImGui::Columns(1);
		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction) {
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen |
												 ImGuiTreeNodeFlags_AllowItemOverlap |
												 ImGuiTreeNodeFlags_Framed |
												 ImGuiTreeNodeFlags_SpanAvailWidth;
		if (entity.hasComponent<T>()) {
			auto& component = entity.getComponent<T>();
			
			auto contentRegionAvail = ImGui::GetContentRegionAvail();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{4, 4});
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.0f);
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Dummy({1.0f, 1.0f});
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, "%s", name.c_str());
			ImGui::PopStyleVar(3);

			ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 1.0f, 1.0f, 0.0f));
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
			if (ImGui::Button(STLR_ICON_FA_GEAR, ImVec2{lineHeight, lineHeight})) {
				ImGui::OpenPopup("ComponentSettings");
			}

			ImGui::PopStyleColor(3);

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings")) {
				if (ImGui::MenuItem("Remove Component")) {
					removeComponent = true;
				}
				ImGui::EndPopup();
			}
			
			if (open) {
				uiFunction(component);
				ImGui::Dummy({2.0f, 2.0f});
				ImGui::TreePop();
			}

			if (removeComponent) {
				entity.removeComponent<T>();
			}
		}
	}

	void SceneHierarchyPanel::drawComponent(Entity entity) {
		if (entity.hasComponent<TagComponent>()) {
			auto& tag = entity.getComponent<TagComponent>().tag;
			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strncpy(buffer, tag.c_str(),  sizeof(buffer));

			if (ImGui::InputText("##Tag", buffer, sizeof(buffer))){
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");
		if (ImGui::BeginPopup("AddComponent")) {
			if (ImGui::MenuItem("Camera")) {
				m_SelectionContext.addComponent<CameraComponent>();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Sprite Renderer")) {
				m_SelectionContext.addComponent<SpriteRendererComponent>(glm::vec4{1.0f}, Texture2D::Create({}));
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		
		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component) {
			DrawVec3Control("Translation", component.translation);
			glm::vec3 rotation = glm::degrees(component.rotation);
			DrawVec3Control("Rotation", rotation);
			component.rotation = glm::radians(rotation);
			DrawVec3Control("Scale", component.scale, 1.0f);
		});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component) {
			auto& camera = component.camera;

			ImGui::Checkbox("Primary", &component.primary);

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera.getProjectionType()];
			if (ImGui::BeginCombo("Projection", currentProjectionTypeString)) {
				for (int i = 0; i < 2; i++) {
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected)) {
						currentProjectionTypeString = projectionTypeStrings[i];
						camera.setProjectionType((SceneCamera::ProjectionType)i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}

			if (camera.getProjectionType() == SceneCamera::ProjectionType::Perspective) {
				float perspectiveVerticalFov = camera.getFov();
				if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
					camera.setFov(perspectiveVerticalFov);

				float perspectiveNear = camera.getPerspectiveNear();
				if (ImGui::DragFloat("Near", &perspectiveNear))
					camera.setPerspectiveNear(perspectiveNear);

				float perspectiveFar = camera.getPerspectiveFar();
				if (ImGui::DragFloat("Far", &perspectiveFar))
					camera.setPerspectiveFar(perspectiveFar);
			}

			if (camera.getProjectionType() == SceneCamera::ProjectionType::Orthographic) {
				float orthoSize = camera.getOrthoGraphicSize();
				if (ImGui::DragFloat("Size", &orthoSize))
					camera.setOrthoGraphicSize(orthoSize);

				float orthoNear = camera.getOrthographicNear();
				if (ImGui::DragFloat("Near", &orthoNear))
					camera.setOrthographicNear(orthoNear);

				float orthoFar = camera.getOrthographicFar();
				if (ImGui::DragFloat("Far", &orthoFar))
					camera.setOrthographicFar(orthoFar);

				ImGui::Checkbox("Fixed Aspect Ratio", &component.fixedAspectRatio);
			}
		});

		DrawComponent<SpriteRendererComponent>("Sprite", entity, [](auto& component) {
			ImGui::ColorEdit4("Color", glm::value_ptr(component.color));
			ImGui::DragFloat("Tiling Factor", &component.tilingFactor, 0.1f, 0.0f, 100.0f);
			UI::Image(component.texture, { 200, 200 });
			if (ImGui::BeginDragDropTarget()) {
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("RESOURCE_ITEM")) {
#if defined(_WIN64)
				const wchar_t* path = (const wchar_t*)payload->Data;
#endif
#if defined(__APPLE__) || defined(__linux__)
				char* path = (char*)payload->Data;
#endif
					auto texturePath = std::filesystem::path("Resources") / path;
					component.texture = Texture2D::Create(texturePath.string(), {});
				}
				ImGui::EndDragDropTarget();
			}
		});
	}
}