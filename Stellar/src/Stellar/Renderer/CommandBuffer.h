#pragma once

#include "Stellar/Core.h"

namespace Stellar {
    class STLR_API CommandBuffer {
    public:
        virtual ~CommandBuffer() = default;

        virtual void begin() = 0;
        virtual void end() = 0;
        virtual void submit() = 0;
        virtual void* getActiveCommandBuffer() = 0;

        static CommandBuffer* Create(uint32_t size);

    protected:
        void* m_ActiveCommandBuffer = nullptr;
    };
}