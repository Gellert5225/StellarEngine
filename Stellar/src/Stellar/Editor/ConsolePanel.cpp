#include "stlrpch.h"
#include "ConsolePanel.h"

#include "Stellar/Core/Log.h"
#include "Stellar/ImGui/Colors.h"
#include "Stellar/ImGui/WebFont.h"

#include "imgui_internal.h"

#include <cstdarg>
#include <iomanip>
#include <chrono>

namespace Stellar {
	static ConsolePanel* s_Instance = nullptr;

	static const ImVec4 s_TraceTint = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	static const ImVec4 s_DebugTint = ImVec4(0.0f, 138.0f/255.0f, 198.0f/255.0f, 1.0f);
	static const ImVec4 s_InfoTint = ImVec4(0.0f, 0.7f, 0.0f, 1.0f);
	static const ImVec4 s_WarningTint = ImVec4(1.0f, 0.890196078f, 0.0588235294f, 1.0f);
	static const ImVec4 s_ErrorTint = ImVec4(1.0f, 0.309803922f, 0.309803922f, 1.0f);

	ConsolePanel::ConsolePanel() {
		STLR_CORE_ASSERT(s_Instance == nullptr, "");
		s_Instance = this;

		m_MessageBuffer.reserve(500);
	}

	ConsolePanel::~ConsolePanel() {
		s_Instance = nullptr;
	}

	void ConsolePanel::onEvent(Event& event) {

	}

	void ConsolePanel::onImGuiRender(bool& isOpen) {
		ImGui::Begin("Log");
		ImVec2 consoleSize = ImGui::GetContentRegionAvail();
		consoleSize.y -= 32.0f;
		renderMenu({ consoleSize.x, 20.0f });
		renderConsole(consoleSize);
		
		ImGui::End();
	}

	void ConsolePanel::renderMenu(const ImVec2& size) {
		//UI::ScopedStyleStack frame(ImGuiStyleVar_FrameBorderSize, 0.0f, ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::BeginChild("Toolbar", size);

		if (ImGui::Button("Clear", { 50.0f, 20.0f })) {
			std::scoped_lock<std::mutex> lock(m_MessageBufferMutex);
			m_MessageBuffer.clear();
		}

		ImGui::SameLine();

		const auto& style = ImGui::GetStyle();
		ImVec4 textColor = m_ClearOnPlay ? style.Colors[ImGuiCol_Text] : style.Colors[ImGuiCol_TextDisabled];

		{
			const ImVec2 buttonSize(20.0f, 20.0f);

			ImGui::SameLine(ImGui::GetContentRegionAvail().x - 80.0f, 0.0f);
			textColor = (m_MessageFilters & (int16_t)ConsoleMessageFlags::Trace) ? s_TraceTint : style.Colors[ImGuiCol_TextDisabled];
			ImGui::PushStyleColor(ImGuiCol_Text, textColor);
			ImGui::PushStyleColor(ImGuiCol_Button, getToolbarButtonColor(m_MessageFilters & (int16_t)ConsoleMessageFlags::Trace));

			if (ImGui::Button(STLR_ICON_FA_LOCATION_DOT, buttonSize))
				m_MessageFilters ^= (int16_t)ConsoleMessageFlags::Trace;

			ImGui::PopStyleColor(2);

			ImGui::SameLine();
			textColor = (m_MessageFilters & (int16_t)ConsoleMessageFlags::Info) ? s_InfoTint : style.Colors[ImGuiCol_TextDisabled];
			ImGui::PushStyleColor(ImGuiCol_Text, textColor);
			ImGui::PushStyleColor(ImGuiCol_Button, getToolbarButtonColor(m_MessageFilters & (int16_t)ConsoleMessageFlags::Info));

			if (ImGui::Button(STLR_ICON_FA_CIRCLE_INFO, buttonSize))
				m_MessageFilters ^= (int16_t)ConsoleMessageFlags::Info;

			ImGui::PopStyleColor(2);

			ImGui::SameLine();
			textColor = (m_MessageFilters & (int16_t)ConsoleMessageFlags::Warning) ? s_WarningTint : style.Colors[ImGuiCol_TextDisabled];
			ImGui::PushStyleColor(ImGuiCol_Text, textColor);
			ImGui::PushStyleColor(ImGuiCol_Button, getToolbarButtonColor(m_MessageFilters & (int16_t)ConsoleMessageFlags::Warning));

			if (ImGui::Button(STLR_ICON_FA_TRIANGLE_EXCLAMATION, buttonSize))
				m_MessageFilters ^= (int16_t)ConsoleMessageFlags::Warning;

			ImGui::PopStyleColor(2);

			ImGui::SameLine();
			textColor = (m_MessageFilters & (int16_t)ConsoleMessageFlags::Error) ? s_ErrorTint : style.Colors[ImGuiCol_TextDisabled];
			ImGui::PushStyleColor(ImGuiCol_Text, textColor);
			ImGui::PushStyleColor(ImGuiCol_Button, getToolbarButtonColor(m_MessageFilters & (int16_t)ConsoleMessageFlags::Error));
			if (ImGui::Button(STLR_ICON_FA_CIRCLE_EXCLAMATION, buttonSize))
				m_MessageFilters ^= (int16_t)ConsoleMessageFlags::Error;
			
			ImGui::PopStyleColor(2);

			ImGui::SameLine();
			textColor = (m_MessageFilters & (int16_t)ConsoleMessageFlags::Debug) ? s_DebugTint : style.Colors[ImGuiCol_TextDisabled];
			ImGui::PushStyleColor(ImGuiCol_Text, textColor);
			ImGui::PushStyleColor(ImGuiCol_Button, getToolbarButtonColor(m_MessageFilters & (int16_t)ConsoleMessageFlags::Debug));
			if (ImGui::Button(STLR_ICON_FA_BUG, buttonSize))
				m_MessageFilters ^= (int16_t)ConsoleMessageFlags::Debug;
			
			ImGui::PopStyleColor(2);
		}
		// STLR_ICON_MAP_MARKER

		ImGui::EndChild();
	}

	void ConsolePanel::renderConsole(const ImVec2& size) {
		static const char* columns[] = { "Type" };

		float edgeOffset = 4.0f;

		ImGuiTableFlags flags = ImGuiTableFlags_NoPadInnerX
			| ImGuiTableFlags_NoPadOuterX
			| ImGuiTableFlags_Resizable
			| ImGuiTableFlags_Reorderable
			| ImGuiTableFlags_ScrollY
			| ImGuiTableFlags_RowBg;

		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImColor backgroundColor = ImColor(Colors::Theme::background);

		ImGui::PushStyleColor(ImGuiCol_TableRowBg, backgroundColor.Value);
		ImGui::PushStyleColor(ImGuiCol_TableRowBgAlt, backgroundColor.Value);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, backgroundColor.Value);

		if (ImGui::BeginTable("Console", 1, flags, size)) {
			const float cursorX = ImGui::GetCursorScreenPos().x;

			ImGui::TableSetupColumn(columns[0]);

			//std::scoped_lock<std::mutex> lock(m_MessageBufferMutex);
			
			float scrollY = ImGui::GetScrollY();
			if (scrollY < m_PreviousScrollY)
				m_EnableScrollToLatest = false;

			if (scrollY >= ImGui::GetScrollMaxY())
				m_EnableScrollToLatest = true;

			m_PreviousScrollY = scrollY;

			float rowHeight = 24.0f;
			for (uint32_t i = 0; i < m_MessageBuffer.size(); i++) {
				const auto& msg = m_MessageBuffer[i];

				if (!(m_MessageFilters & (int16_t)msg.Flags))
					continue;

				ImGui::PushID(&msg);

				const bool clicked = tableRowClickable(msg.ShortMessage.c_str(), rowHeight);
				auto color = getMessageColor(msg);
				std::string messageType(getMessageType(msg));
				std::stringstream timeString;
				tm timeBuffer;
				timeBuffer = *localtime(&msg.Time);
				timeString << std::put_time(&timeBuffer, "%T");

				std::string timeMessage = "[" + timeString.str() + "]" + messageType;

				ImGuiIO& io = ImGui::GetIO();
				auto boldFont = io.Fonts->Fonts[4];
				auto regFont = io.Fonts->Fonts[3];

				ImGui::PushFont(boldFont);
				ImGui::TextColored(color, "%s", timeMessage.c_str());
				ImGui::PopFont();

				ImGui::SameLine();

				ImGui::PushFont(regFont);
				ImGui::TextColored(color, "%s", msg.LongMessage.c_str());
				ImGui::PopFont();
				
				ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 4.0f);

				if (i == m_MessageBuffer.size() - 1 && m_ScrollToLatest) {
					ImGui::ScrollToItem();
					m_ScrollToLatest = false;
				}

				// if (clicked) {
				// 	ImGui::OpenPopup("Detailed Message");
				// 	ImVec2 size = ImGui::GetMainViewport()->Size;
				// 	ImGui::SetNextWindowSize({ size.x * 0.5f, size.y * 0.5f });
				// 	ImGui::SetNextWindowPos({ size.x / 2.0f, size.y / 2.5f }, 0, { 0.5, 0.5 });
				// 	m_DetailedPanelOpen = true;
				// }

				// if (m_DetailedPanelOpen) {
				// 	UI::ScopedStyle windowPadding(ImGuiStyleVar_WindowPadding, ImVec2(4.0f, 4.0f));
				// 	UI::ScopedStyle framePadding(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 8.0f));

				// 	if (ImGui::BeginPopupModal("Detailed Message", &m_DetailedPanelOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize)) {
				// 		ImGui::TextWrapped(msg.LongMessage.c_str());
				// 		ImGui::EndPopup();
				// 	}
				// }

				ImGui::PopID();
			}
			ImGui::EndTable();
		}
		ImGui::PopStyleColor(3);
		ImGui::PopStyleVar(2);
	}

	const char* ConsolePanel::getMessageType(const ConsoleMessage& message) const {
		if (message.Flags & (int16_t)ConsoleMessageFlags::Info) return "[INFO] ";
		if (message.Flags & (int16_t)ConsoleMessageFlags::Trace) return "[TRACE]";
		if (message.Flags & (int16_t)ConsoleMessageFlags::Warning) return "[WARN] ";
		if (message.Flags & (int16_t)ConsoleMessageFlags::Error) return "[ERROR]";
		if (message.Flags & (int16_t)ConsoleMessageFlags::Debug) return "[DEBUG]";
		return "Unknown";
	}

	const ImVec4& ConsolePanel::getMessageColor(const ConsoleMessage& message) const {
		//if (message.Flags & (int16_t)ConsoleMessageFlags::Info) return s_InfoButtonOnTint;
		if (message.Flags & (int16_t)ConsoleMessageFlags::Warning) return s_WarningTint;
		if (message.Flags & (int16_t)ConsoleMessageFlags::Error) return s_ErrorTint;
		if (message.Flags & (int16_t)ConsoleMessageFlags::Trace) return s_TraceTint;
		if (message.Flags & (int16_t)ConsoleMessageFlags::Debug) return s_DebugTint;
		return s_InfoTint;
	}

	ImVec4 ConsolePanel::getToolbarButtonColor(const bool value) const {
		const auto& style = ImGui::GetStyle();
		return value ? style.Colors[ImGuiCol_Header] : style.Colors[ImGuiCol_FrameBg];
	}

	void ConsolePanel::PushMessage(const ConsoleMessage& message) {
		if (s_Instance == nullptr)
			return;

		{
			std::scoped_lock<std::mutex> lock(s_Instance->m_MessageBufferMutex);
			s_Instance->m_MessageBuffer.push_back(message);
		}

		if (s_Instance->m_EnableScrollToLatest)
			s_Instance->m_ScrollToLatest = true;
	}

	bool ConsolePanel::tableRowClickable(const char* id, float rowHeight) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		window->DC.CurrLineSize.y = rowHeight;

		ImGui::TableNextRow(0, rowHeight);
		ImGui::TableNextColumn();

		window->DC.CurrLineTextBaseOffset = 3.0f;
		const ImVec2 rowAreaMin = ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), 0).Min;
		const ImVec2 rowAreaMax = { ImGui::TableGetCellBgRect(ImGui::GetCurrentTable(), ImGui::TableGetColumnCount() - 1).Max.x, rowAreaMin.y + rowHeight };

		ImGui::PushClipRect(rowAreaMin, rowAreaMax, false);

		bool isRowHovered, held;
		bool isRowClicked = ImGui::ButtonBehavior(ImRect(rowAreaMin, rowAreaMax), ImGui::GetID(id),
			&isRowHovered, &held, ImGuiButtonFlags_AllowItemOverlap);

		ImGui::SetItemAllowOverlap();
		ImGui::PopClipRect();

		return isRowClicked;
	}

}