#pragma once

#include "Stellar/Core.h"

namespace Stellar {
    class STLR_API VertexBuffer {
    public:
        virtual ~VertexBuffer() = default;

        virtual void copy(const VertexBuffer& dst) = 0;
        [[nodiscard]] virtual void* getBuffer() const = 0;

        static VertexBuffer* Create(uint64_t size, const void* data = nullptr);

    protected:
        explicit VertexBuffer(uint64_t size) : m_Size(size) {}
        uint64_t m_Size = 0;
    };

    class STLR_API IndexBuffer {
    public:
        virtual ~IndexBuffer() = default;

        virtual void copy(const IndexBuffer& dst) = 0;
        [[nodiscard]] virtual void* getBuffer() const = 0;

        static IndexBuffer* Create(uint64_t size, const void* data = nullptr);
    protected:
        explicit IndexBuffer(uint64_t size) : m_Size(size) {}
        uint64_t m_Size = 0;
    };
}