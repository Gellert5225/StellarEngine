#include <Stellar/Core/EntryPoint.h>

#include "EditorLayer.h"

namespace Stellar {
	class StellarEditor: public Application {
		public:
			StellarEditor() {
				pushLayer(ImGuiLayer::Create());
				pushLayer(new EditorLayer());
			}
			~StellarEditor() override = default;
	};

	Application* CreateApplication() {
		return new StellarEditor();
	}
}