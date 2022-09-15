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
    }

    void MetalSwapChain::init() {
        createSwapChain();
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

    void MetalSwapChain::beginFrame() {
        auto* drawable = reinterpret_cast<CA::MetalDrawable*>(nextDrawable(m_MetalSwapChain));

        if (drawable == nullptr) {
            STLR_CORE_ASSERT(false, "Failed to acquire Metal Drawable");
        }
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
}