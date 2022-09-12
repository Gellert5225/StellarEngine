#pragma once

#include "Stellar/Core.h"

namespace Stellar {
    enum class BufferType {
        Vertex,
        Index,
        Uniform
    };

    class STLR_API Buffer {
    public:
        virtual ~Buffer() = default;

        virtual void copy(const Buffer& dst) = 0;
        [[nodiscard]] virtual void* getBuffer() const = 0;

        static Buffer* Create(BufferType type, uint64_t size, const void* data = nullptr);

        virtual void map(void** data) = 0;
        virtual void unMap() = 0;
        virtual void write(void* dst, const void* src) = 0;
    protected:
        explicit Buffer(uint64_t size) : m_Size(size) {}
        uint64_t m_Size = 0;
    };
}