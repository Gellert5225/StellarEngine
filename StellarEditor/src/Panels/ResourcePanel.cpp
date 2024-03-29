#include "stlrpch.h"

#include "ResourcePanel.h"
#include <Stellar/ImGui/WebFont.h>
#include <Stellar/ImGui/ImGuiLayer.h>

#include <imgui.h>

namespace Stellar {

	static const std::filesystem::path s_ResourcePath = "Resources";

	ResourcePanel::ResourcePanel() : m_CurrentDir(s_ResourcePath) {
		TextureSpecification spec{};
		spec.isImGuiTexture = true;
		m_FolderIcon = Texture2D::Create("Resources/Icons/folder.png", spec);
		m_FileIcon = Texture2D::Create("Resources/Icons/file.png", spec);
		m_ArrowBackIcon = Texture2D::Create("Resources/Icons/arrow_back.png", spec);
	}

	void ResourcePanel::onImGuiRender(bool &isOpen) {
		ImGui::Begin("Resources");

		if (m_CurrentDir != std::filesystem::path(s_ResourcePath)) {
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1,1,1,0));
			if (ImGui::ImageButton(m_ArrowBackIcon->getImGuiTextureID(), {20, 20})) {
				m_CurrentDir = m_CurrentDir.parent_path();
			}
			ImGui::PopStyleColor();
		}

		float padding = 10.0f;
		float thumbnailSize = 80.0f;
		float cellSize = thumbnailSize + padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize) < 1 ? 1 : (int)(panelWidth / cellSize);

		ImGui::Columns(columnCount, 0, false);

		for(auto& dir: std::filesystem::directory_iterator(m_CurrentDir)) {
			const auto& path = dir.path();
			auto relativePath = std::filesystem::relative(dir.path(), s_ResourcePath);
			auto fileName = relativePath.filename().string();
			ImGui::PushID(fileName.c_str());
			//STLR_CORE_INFO("Extension: {0}", path.extension().string().c_str());
			//STLR_CORE_INFO("path: {0}", path.string());

			STLR_Ptr<Texture2D> icon = dir.is_directory() ? m_FolderIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
			if (path.extension().string() == ".png" || path.extension().string() == ".jpg") {
				auto texture = m_PreviewTextuers[path.string()];
				if (!texture) {
					TextureSpecification spec{};
					spec.isImGuiTexture = true;
					m_PreviewTextuers[path.string()] = Texture2D::Create(path.string(), spec);
				}
				texture = m_PreviewTextuers[path.string()];
				ImGui::ImageButton(texture->getImGuiTextureID(), {thumbnailSize, thumbnailSize});
			} else {
				ImGui::ImageButton(icon->getImGuiTextureID(), {thumbnailSize, thumbnailSize});
			}

			if (ImGui::BeginDragDropSource()) {
				const wchar_t* itemPath = (const wchar_t*)relativePath.c_str();
				ImGui::SetDragDropPayload("RESOURCE_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor(2);
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (dir.is_directory()) {
					m_CurrentDir /= path.filename();
				}
			}
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + cellSize / 2 - ImGui::CalcTextSize(fileName.c_str()).x / 2);
			ImGui::Text("%s", fileName.c_str());

			ImGui::NextColumn();
			ImGui::PopID();
		}

		ImGui::Columns(1);
		ImGui::End();
	}
}