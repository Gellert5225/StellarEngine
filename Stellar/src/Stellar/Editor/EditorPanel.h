#pragma once

#include "Stellar/Core/STLRBase.h"
#include "Stellar/Core/Core.h"
#include "Stellar/Scene/Scene.h"
#include "Stellar/Events/Event.h"

namespace Stellar {
	class EditorPanel : public STLR_Base {
	public:
		virtual ~EditorPanel() = default;

		virtual void onImGuiRender(bool& isOpen) = 0;
		virtual void onEvent(Event& e) {}
		virtual void setSceneContext(const STLR_Ptr<Scene>& context){}
	};
}