#include <Stellar.h>

#include "ExampleLayer.h"
#include "ExampleMetalLayer.h"

class Sandbox: public Stellar::Application {
    public:
        Sandbox() {
            //pushLayer(new ExampleLayer());
            //pushLayer(Stellar::ImGuiLayer::Create());
            pushLayer(new ExampleMetalLayer());
        }
        ~Sandbox() override = default;
};

Stellar::Application* Stellar::CreateApplication() {
	return new Sandbox();
}