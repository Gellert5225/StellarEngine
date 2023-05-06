#pragma once

#include "EditorPanel.h"

#include "ConsoleMessage.h"

#include <imgui.h>
#include <mutex>

namespace Stellar {
	class ConsolePanel : public EditorPanel {
	public:
		ConsolePanel();
		~ConsolePanel();

		virtual void onEvent(Event& e) override;
		virtual void onImGuiRender(bool& isOpen) override;

	private:
		void renderMenu(const ImVec2& size);
		void renderConsole(const ImVec2& size);
		const char* getMessageType(const ConsoleMessage& message) const;
		const ImVec4& getMessageColor(const ConsoleMessage& message) const;
		ImVec4 getToolbarButtonColor(const bool value) const;

	private:
		static void PushMessage(const ConsoleMessage& message);
		bool tableRowClickable(const char* id, float rowHeight);
	private:
		bool m_ClearOnPlay = true;

		std::mutex m_MessageBufferMutex;
		std::vector<ConsoleMessage> m_MessageBuffer;

		bool m_EnableScrollToLatest = true;
		bool m_ScrollToLatest = false;
		float m_PreviousScrollY = 0.0f;

		int16_t m_MessageFilters = (int16_t)ConsoleMessageFlags::All;

		bool m_DetailedPanelOpen = false;
	private:
		friend class ConsoleSink;
	};

}