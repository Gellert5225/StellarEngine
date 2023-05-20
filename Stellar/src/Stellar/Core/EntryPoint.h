#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/Log.h"
#include "Stellar/Core/Application.h"

#if defined(__APPLE__)
#include "Stellar/Platform/Metal/MetalAPI.h"
#endif

extern Stellar::Application* Stellar::CreateApplication();

int main(int argc, char** argv) {
#if defined (__APPLE__)
	NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();
#endif
	Stellar::Log::Init();
	STLR_CORE_INFO("Welcome to Stellar Engine");
	STLR_CORE_WARN("This engine is still under active development!");
	
	auto app = Stellar::CreateApplication();

	STLR_CONSOLE_LOG_INFO("Welcome to Stellar Engine");
	STLR_CONSOLE_LOG_WARN("This engine is still under active development!");

	app->run();
#if defined (__APPLE__)
    pool->release();
#endif
	delete app;

	return 0;
}
