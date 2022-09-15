#include "stlrpch.h"

#include "MetalSwapChain.h"
#include "Stellar/Platform/Metal/Device/MetalDevice.h"

#include "Stellar/Application.h"
#include "Stellar/Log.h"

namespace Stellar {

    MetalSwapChain::MetalSwapChain() {
        init();
    }

    MetalSwapChain::~MetalSwapChain() {
        m_MetalSwapChain = nullptr;
        //m_CommandBuffer->release();
    }

    void MetalSwapChain::init() {
        createSwapChain();
        createRenderPass();
        createCommandBuffer();
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

    }

    void MetalSwapChain::createRenderPass() {
        m_Drawable = reinterpret_cast<CA::MetalDrawable*>(nextDrawable(m_MetalSwapChain));

        if (m_Drawable == nullptr) {
            STLR_CORE_ASSERT(false, "Failed to acquire Metal Drawable");
        }

//        m_RenderPass = MTL::RenderPassDescriptor::alloc()->init();
//        auto colorAttachment = m_RenderPass->colorAttachments()->object(0);
//
//        colorAttachment->setClearColor(MTL::ClearColor::Make(0, 0, 0, 0.25));
//        colorAttachment->setLoadAction(MTL::LoadActionClear);
//        colorAttachment->setStoreAction(MTL::StoreActionStore);
//        colorAttachment->setTexture(m_Drawable->texture());
    }

    void MetalSwapChain::createCommandBuffer() {
        //m_CommandBuffer = MetalDevice::GetInstance()->getCommandQueue()->commandBuffer();
    }

    void MetalSwapChain::beginFrame() {
    }

    void MetalSwapChain::present() {

    }

    void MetalSwapChain::onResize() {

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
}