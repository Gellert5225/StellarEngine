#include <Stellar.h>

#include "ExampleLayer.h"

class Sandbox: public Stellar::Application {
    public:
        Sandbox() {
            pushLayer(new ExampleLayer());
            pushLayer(new Stellar::ImGuiLayer());
        }
        ~Sandbox() {}
};

Stellar::Application* Stellar::CreateApplication() {
	return new Sandbox();
}