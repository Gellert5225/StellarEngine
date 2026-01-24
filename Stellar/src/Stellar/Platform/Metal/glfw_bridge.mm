#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

void* createLayer(GLFWwindow* window, double width, double height, void* device) {
    NSWindow* nswindow = glfwGetCocoaWindow(window);
    CGFloat scale = nswindow.backingScaleFactor;
    
    CGSize size = {};
    size.height = height * scale;
    size.width = width * scale;

    CAMetalLayer* layer = [CAMetalLayer layer];
    layer.device = (__bridge id<MTLDevice>) device;
    layer.pixelFormat = MTLPixelFormatBGRA8Unorm;
    layer.drawableSize = size;
    layer.contentsScale = scale;
	layer.displaySyncEnabled = false;

    nswindow.contentView.layer = layer;
    nswindow.contentView.wantsLayer = YES;

    return (__bridge void *)layer;
}

void* nextDrawable(void* layer) {
    CAMetalLayer* metalLayer = (__bridge CAMetalLayer*) layer;
    return (__bridge void *)[metalLayer nextDrawable];
}
