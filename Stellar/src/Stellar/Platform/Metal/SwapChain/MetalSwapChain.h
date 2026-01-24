#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Renderer/SwapChain.h"
#include "Stellar/Platform/Metal/MetalAPI.h"

#include <GLFW/glfw3.h>

float getScale(GLFWwindow* window);
void setWindowContentView(GLFWwindow* window, CA::MetalLayer* layer);

namespace Stellar {
    class MetalSwapChain : public SwapChain {
    public:
        MetalSwapChain();
        ~MetalSwapChain() override;

        void beginFrame() override;
        void present() override;
        void onResize() override;

        [[nodiscard]] SwapChainExtent2D getSwapChainExtent() const override;
        [[nodiscard]] uint32_t getCurrentFrameIndex() const override;
        
        CA::MetalDrawable* getCurrentFrameBuffer();
        MTL::RenderPassDescriptor* getRenderPass();
        MTL::RenderPassDescriptor* getImGuiRenderPass();
        MTL::CommandBuffer* getCommandBuffer();
        void* getSwapChain();
    private:
        CA::MetalLayer* m_MetalSwapChain;
        CA::MetalDrawable* m_Drawable;
        MTL::RenderPassDescriptor* m_RenderPass;
        MTL::RenderPassDescriptor* m_ImGuiRenderPass;
        MTL::CommandBuffer* m_CommandBuffer = nullptr;
        SwapChainExtent2D m_SwapChainExtant;

		uint32_t m_CurrentFrameIndex = 0;

        void init();
        void createSwapChain();
        void createRenderPass();
        void createCommandBuffer();

        CA::MetalLayer* createLayer(GLFWwindow* window, double width, double height, MTL::Device* device);
        CA::MetalDrawable* nextDrawable(CA::MetalLayer* layer);
    };
}