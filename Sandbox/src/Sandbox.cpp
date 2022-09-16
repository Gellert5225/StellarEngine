#include <Stellar.h>

#include "ExampleLayer.h"
#include "ExampleMetalLayer.h"

class Sandbox: public Stellar::Application {
    public:
        Sandbox() {
            pushLayer(new ExampleLayer());
            pushLayer(new Stellar::ImGuiLayer());
            //pushLayer(new ExampleMetalLayer());
        }
        ~Sandbox() override = default;
};

Stellar::Application* Stellar::CreateApplication() {
	return new Sandbox();
}