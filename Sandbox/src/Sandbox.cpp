#include <Stellar.h>

#include "ExampleLayer.h"
#include "ExampleMetalLayer.h"

class Sandbox: public Stellar::Application {
    public:
        Sandbox() {
            #if defined __linux__ || defined _WIN64 
            pushLayer(new ExampleLayer());
            #endif

            #if defined __APPLE__
            pushLayer(new ExampleMetalLayer());
            #endif

            pushLayer(Stellar::ImGuiLayer::Create());
        }
        ~Sandbox() override = default;
};

Stellar::Application* Stellar::CreateApplication() {
	return new Sandbox();
}