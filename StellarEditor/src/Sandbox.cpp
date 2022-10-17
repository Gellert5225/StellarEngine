#include <Stellar.h>

#include "Sandbox2D.h"

class Sandbox: public Stellar::Application {
    public:
        Sandbox() {
            pushLayer(Stellar::ImGuiLayer::Create());
            pushLayer(new Sandbox2D());
        }
        ~Sandbox() override = default;
};

Stellar::Application* Stellar::CreateApplication() {
	return new Sandbox();
}