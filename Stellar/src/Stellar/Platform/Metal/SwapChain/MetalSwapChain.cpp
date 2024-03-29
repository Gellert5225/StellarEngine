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
        float xscale, yscale;
        glfwGetWindowContentScale(Application::Get().getWindow().getGLFWWindow(), &xscale, &yscale);

        int width, height;
        glfwGetFramebufferSize(Application::Get().getWindow().getGLFWWindow(), &width, &height);
        m_MetalSwapChain = createLayer(Application::Get().getWindow().getGLFWWindow(),
                                       width * xscale,
                                       height * yscale,
                                       MetalDevice::GetInstance()->getDevice());
        m_SwapChainExtant.width = width * xscale;
        m_SwapChainExtant.height = height * yscale;

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
		m_CurrentFrameIndex = (m_CurrentFrameIndex + 1) % Renderer::MAX_FRAMES_IN_FLIGHT;
		
        m_Drawable->present();
        //m_Drawable->release();
    }

    void MetalSwapChain::onResize() {
        m_MetalSwapChain = nullptr;
        m_RenderPass->release();
        createSwapChain();
        createRenderPass();
    }

    SwapChain::SwapChainExtent2D MetalSwapChain::getSwapChainExtent() const {
        return m_SwapChainExtant;
    }

    uint32_t MetalSwapChain::getCurrentFrameIndex() const {
        return m_CurrentFrameIndex;
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
