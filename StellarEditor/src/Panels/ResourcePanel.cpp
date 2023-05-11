#include "stlrpch.h"

#include "ResourcePanel.h"
#include <Stellar/ImGui/WebFont.h>
#include <Stellar/ImGui/ImGuiLayer.h>

#include <imgui.h>

namespace Stellar {

	static const std::filesystem::path s_ResourcePath = "Resources";

	ResourcePanel::ResourcePanel() : m_CurrentDir(s_ResourcePath) {
		m_FolderIcon = Texture2D::Create("Resources/Icons/folder.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/file.png");
		m_ArrowBackIcon = Texture2D::Create("Resources/Icons/arrow_back.png");
	}

	void ResourcePanel::onImGuiRender(bool &isOpen) {
		ImGui::Begin("Resources");

		if (m_CurrentDir != std::filesystem::path(s_ResourcePath)) {
			UI::ImageButton(m_ArrowBackIcon, {20, 20});
			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
				m_CurrentDir = m_CurrentDir.parent_path();
			}
		}

		float padding = 1.0f;
		float thumbnailSize = 100.0f;
		float cellSize = thumbnailSize + padding;
		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize) < 1 ? 1 : (int)(panelWidth / cellSize);

		ImGui::Columns(columnCount, 0, false);

		for(auto& dir: std::filesystem::directory_iterator(m_CurrentDir)) {
			const auto& path = dir.path();
			auto relativePath = std::filesystem::relative(dir.path(), s_ResourcePath);
			auto fileName = relativePath.filename().string(); 

			STLR_Ptr<Texture2D> icon = dir.is_directory() ? m_FolderIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
			UI::ImageButton(icon, {thumbnailSize, thumbnailSize});
			ImGui::PopStyleColor(2);
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
				if (dir.is_directory()) {
					m_CurrentDir /= path.filename();
				}
			}
			ImGui::Text(fileName.c_str());

			ImGui::NextColumn();
		}

		ImGui::Columns(1);
		ImGui::End();
	}
}