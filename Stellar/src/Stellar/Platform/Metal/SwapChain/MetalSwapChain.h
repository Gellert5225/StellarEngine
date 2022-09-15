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
        void* getSwapChain();
    private:
        void* m_MetalSwapChain;

        void init();
        void createSwapChain();
    };
}