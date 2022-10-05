#include "stlrpch.h"

#include "MetalSwapChain.h"
#include "Stellar/Platform/Metal/Device/MetalDevice.h"

#include "Stellar/Core/Application.h"
#include "Stellar/Core/Log.h"

namespace Stellar {

    MetalSwapChain::MetalSwapChain() {
        init();
    }

    MetalSwapChain::~MetalSwapChain() {
        m_MetalSwapChain = nullptr;
        m_CommandBuffer->release();
        m_RenderPass->release();
    }

    void MetalSwapChain::init() {
        createSwapChain();
        createRenderPass();
        //createCommandBuffer();
    }

    void MetalSwapChain::createSwapChain() {
        NS::AutoreleasePool* pPool = NS::AutoreleasePool::alloc()->init();
        float xscale, yscale;
        glfwGetWindowContentScale(Application::Get().getWindow().getGLFWWindow(), &xscale, &yscale);

        int width, height;
        glfwGetFramebufferSize(Application::Get().getWindow().getGLFWWindow(), &width, &height);
        m_MetalSwapChain = createLayer(Application::Get().getWindow().getGLFWWindow(),
                                       width * xscale,
                                       height * yscale,
                                       MetalDevice::GetInstance()->getDevice());

        pPool->release();

    }

    void MetalSwapChain::createRenderPass() {
        m_RenderPass = MTL::RenderPassDescriptor::alloc()->init();
        //m_ImGuiRenderPass = MTL::RenderPassDescriptor::alloc()->init();
    }

    void MetalSwapChain::createCommandBuffer() {
        m_CommandBuffer = MetalDevice::GetInstance()->getCommandQueue()->commandBuffer();
    }

    void MetalSwapChain::beginFrame() {
        m_Drawable = reinterpret_cast<CA::MetalDrawable*>(nextDrawable(m_MetalSwapChain));

        if (m_Drawable == nullptr) {
            STLR_CORE_ASSERT(false, "Failed to acquire Metal Drawable");
        }
    }

    void MetalSwapChain::present() {
        m_Drawable->present();
        m_Drawable->release();
    }

    void MetalSwapChain::onResize() {
        m_MetalSwapChain = nullptr;
        createSwapChain();
    }

    SwapChain::SwapChainExtent2D MetalSwapChain::getSwapChainExtent() const {
        return SwapChain::SwapChainExtent2D();
    }

    uint32_t MetalSwapChain::getCurrentFrameIndex() const {
        return 0;
    }

    void* MetalSwapChain::getSwapChain() {
        return m_MetalSwapChain;
    }

    CA::MetalDrawable *MetalSwapChain::getCurrentFrameBuffer() {
        return m_Drawable;
    }

    MTL::RenderPassDescriptor *MetalSwapChain::getRenderPass() {
        return m_RenderPass;
    }

    MTL::CommandBuffer *MetalSwapChain::getCommandBuffer() {
        return m_CommandBuffer;
    }

    MTL::RenderPassDescriptor *MetalSwapChain::getImGuiRenderPass() {
        return m_ImGuiRenderPass;
    }
}