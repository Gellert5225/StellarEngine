#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include "Stellar/Platform/Metal/MetalAPI.h"

#import <Cocoa/Cocoa.h>
#import <QuartzCore/QuartzCore.h>

float getScale(GLFWwindow* window) {
    @autoreleasepool {
        NSWindow* nswindow = glfwGetCocoaWindow(window);
        return static_cast<float>(nswindow.backingScaleFactor);
    }
}

void setWindowContentView(GLFWwindow* window, CA::MetalLayer* layer) {
    @autoreleasepool {
        NSWindow* nswindow = glfwGetCocoaWindow(window);
        CAMetalLayer* objcLayer = (__bridge CAMetalLayer*)layer;
        objcLayer.contentsScale = nswindow.backingScaleFactor;
        nswindow.contentView.layer = (__bridge CAMetalLayer*)layer;
        nswindow.contentView.wantsLayer = YES;
    }
}
