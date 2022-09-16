#pragma once

#include "Stellar/Core.h"
#include "Stellar/Renderer/SwapChain.h"
#include "Stellar/Platform/Metal/MetalAPI.h"

#include <GLFW/glfw3.h>

void* createLayer(GLFWwindow* window, double width, double height, void* device);
void* nextDrawable(void* layer);

namespace Stellar {
    class STLR_API MetalSwapChain : public SwapChain {
    public:
        MetalSwapChain();
        ~MetalSwapChain() override;

        void beginFrame() override;
        void present() override;
        void onResize() override;

        [[nodiscard]] SwapChainExtent2D getSwapChainExtent() const override;
        [[nodiscard]] uint32_t getCurrentFrameIndex() const override;
#if defined(__APPLE__)
        CA::MetalDrawable* getCurrentFrameBuffer();
        MTL::RenderPassDescriptor* getRenderPass();
        MTL::RenderPassDescriptor* getImGuiRenderPass();
        MTL::CommandBuffer* getCommandBuffer();
#endif
        void* getSwapChain();
    private:
        void* m_MetalSwapChain;
#if defined(__APPLE__)
        CA::MetalDrawable* m_Drawable;
        MTL::RenderPassDescriptor* m_RenderPass;
        MTL::RenderPassDescriptor* m_ImGuiRenderPass;
        MTL::CommandBuffer* m_CommandBuffer = nullptr;
#endif
        void init();
        void createSwapChain();
        void createRenderPass();
        void createCommandBuffer();
    };
}