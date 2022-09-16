#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

void* createLayer(GLFWwindow* window, double width, double height, void* device) {
    @autoreleasepool {
        CGSize size = {};
        size.height = height;
        size.width = width;

        CAMetalLayer* layer = [CAMetalLayer layer];
        layer.device = (__bridge id<MTLDevice>) device;
        layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
        layer.drawableSize = size;

        NSWindow* nswindow = glfwGetCocoaWindow(window);
        nswindow.contentView.layer = layer;
        nswindow.contentView.wantsLayer = YES;

        return layer;
    }
}

void* nextDrawable(void* layer) {
    CAMetalLayer* metalLayer = (__bridge CAMetalLayer*) layer;
    return [metalLayer nextDrawable];
}