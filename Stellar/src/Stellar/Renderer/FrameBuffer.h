#pragma once

#include "Stellar/Core/Core.h"

namespace Stellar {
    struct FrameBufferSpec {
        uint32_t width, height;
        bool swapChainTarget = false;
    };

    class STLR_API FrameBuffer {
    public:
        virtual ~FrameBuffer() = default;

        static FrameBuffer* Create(const FrameBufferSpec& spec);

        virtual void resize(uint32_t width, uint32_t height) = 0;
        virtual const FrameBufferSpec& getSpecification() const = 0;
    };
} 