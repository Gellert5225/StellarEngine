#pragma once

#include <Stellar/Editor/EditorPanel.h>
#include <Stellar/Renderer/Texture.h>

#include <filesystem>

namespace Stellar {
	class ResourcePanel : public EditorPanel {
	public:
		ResourcePanel();

		virtual void onImGuiRender(bool& isOpen) override;
	private:
		std::filesystem::path m_CurrentDir;

		STLR_Ptr<Texture2D> m_FolderIcon;
		STLR_Ptr<Texture2D> m_FileIcon;
		STLR_Ptr<Texture2D> m_ArrowBackIcon;
	};
}