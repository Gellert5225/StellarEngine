#pragma once

#include "Stellar/Core.h"

#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {
    class STLR_API SwapChain {
    public:
        struct SwapChainExtent2D {
            uint32_t width;
            uint32_t height;
        };

        virtual ~SwapChain() = default;

        static SwapChain* Create();

        virtual void beginFrame() = 0;
        virtual void present() = 0;
        virtual void onResize() = 0;

        [[nodiscard]] virtual SwapChainExtent2D getSwapChainExtent() const = 0;
        [[nodiscard]] virtual uint32_t getCurrentFrameIndex() const = 0;
    };
}