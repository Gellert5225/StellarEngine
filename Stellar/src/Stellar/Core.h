#pragma once

#include <csignal>

#define GLFW_INCLUDE_VULKAN

#if defined(_WIN64)
    #define EXPORT __declspec(dllexport)
    #define IMPORT __declspec(dllimport)
    #define DEBUG_BREAK __debugbreak()
#elif defined(__linux__)
    #define EXPORT __attribute__((visibility("default")))
    #define IMPORT
    #define DEBUG_BREAK raise(SIGTRAP)
#elif defined(__APPLE__)
    #define EXPORT
    #define IMPORT
    #include <signal.h>
    #define DEBUG_BREAK raise(SIGTRAP)
#endif

#if STLR_BUILD_DLL
    #define STLR_API EXPORT
#else
    #define STLR_API IMPORT
#endif

#define STLR_ASSERT(x, ...) { if(!(x)) { STLR_ERROR("Assertion Failed: {0}", __VA_ARGS__); DEBUG_BREAK;} }
#define STLR_CORE_ASSERT(x, ...) { if(!(x)) { STLR_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); DEBUG_BREAK;} }
#define BIT(x) (1 << x)
#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)