#pragma once

#include "Stellar/Core/Core.h"

namespace Stellar {
    struct FrameBufferSpec {
        uint32_t width, height;
        bool swapChainTarget = false;
    };

    class STLR_API FrameBuffer {
    public:
        static FrameBuffer* Create(const FrameBufferSpec& spec);

        virtual const FrameBufferSpec& getSpecification() const = 0;
    };
} 