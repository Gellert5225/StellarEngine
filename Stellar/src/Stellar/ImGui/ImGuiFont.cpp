#include "stlrpch.h"
#include "ImGuiFont.h"

namespace Stellar::UI {

	static std::unordered_map<std::string, ImFont*> s_Fonts;

	void Fonts::Add(const FontConfiguration& config, bool isDefault) {
		if (s_Fonts.find(config.FontName) != s_Fonts.end()) {
			return;
		}

		ImFontConfig imguiFontConfig;
		imguiFontConfig.MergeMode = config.MergeWithLast;
		auto& io = ImGui::GetIO();
		ImFont* font = io.Fonts->AddFontFromFileTTF(config.FilePath.data(), config.Size, &imguiFontConfig, config.GlyphRanges == nullptr ? io.Fonts->GetGlyphRangesDefault() : config.GlyphRanges);
		s_Fonts[config.FontName] = font;

		if (isDefault)
			io.FontDefault = font;
	}

	ImFont* Fonts::Get(const std::string& fontName) {
		return s_Fonts.at(fontName);
	}

	void Fonts::PushFont(const std::string& fontName) {
		const auto& io = ImGui::GetIO();

		if (s_Fonts.find(fontName) == s_Fonts.end()) {
			ImGui::PushFont(io.FontDefault);
			return;
		}

		ImGui::PushFont(s_Fonts.at(fontName));
	}

	void Fonts::PopFont() {
		ImGui::PopFont();
	}

}
