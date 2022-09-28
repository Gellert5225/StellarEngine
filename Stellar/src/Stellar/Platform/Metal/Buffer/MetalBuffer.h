#pragma once 

#include "Stellar/Core/Core.h"

#include "Stellar/Renderer/Buffer.h"
#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {
    class STLR_API MetalBuffer : public Buffer {
    public:
        explicit MetalBuffer(uint64_t size,
                             const void* data = nullptr);
        ~MetalBuffer() override;

        void copy(const Buffer& dst) override;
        void map(void** data) override;
        void unMap() override;
        void write(void* dst, const void* src) override;

        [[nodiscard]] void* getBuffer() const override;
    private:
        MTL::Buffer* m_Buffer;
    };
}