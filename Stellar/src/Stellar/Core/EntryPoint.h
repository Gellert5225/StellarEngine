#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/Log.h"
#include "Stellar/Core/Application.h"

extern Stellar::Application* Stellar::CreateApplication();

int main(int argc, char** argv) {
	Stellar::Log::Init();
	STLR_CORE_INFO("Welcome to Stellar Engine");
	STLR_CORE_WARN("This engine is still under active development!");
	
	auto app = Stellar::CreateApplication();
	app->run();
	delete app;
}