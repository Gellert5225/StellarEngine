#include "stlrpch.h"
#include "MetalBuffer.h"

#include "Stellar/Platform/Metal/Device/MetalDevice.h"

namespace Stellar {
    MetalBuffer::MetalBuffer(uint64_t size,
                const void* data) : Buffer(size) {
        auto device = MetalDevice::GetInstance()->getDevice();
        m_Buffer = device->newBuffer(size, MTL::ResourceStorageModeManaged);

        write(m_Buffer->contents(), data);
        m_Buffer->didModifyRange( NS::Range::Make(0, m_Buffer->length()));
    }

    MetalBuffer::~MetalBuffer() {
        m_Buffer->release();
    }

    void MetalBuffer::copy(const Buffer& dst) {

    }
    void MetalBuffer::map(void** data) {

    }
    void MetalBuffer::unMap() {

    }
    void MetalBuffer::write(void* dst, const void* src) {
        memcpy(dst, src, m_Size);
    }

    void* MetalBuffer::getBuffer() const { 
        return m_Buffer;
    }
}
