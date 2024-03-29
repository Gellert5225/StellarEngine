#include "stlrpch.h"
#include "MetalBuffer.h"

#include "Stellar/Platform/Metal/Device/MetalDevice.h"
#include "Stellar/Core/Log.h"

namespace Stellar {
    MetalBuffer::MetalBuffer(uint64_t size, const void* data) : Buffer(size) {
        auto device = MetalDevice::GetInstance()->getDevice();
        m_Buffer = device->newBuffer(size, MTL::ResourceStorageModeManaged);

        if (data) {
            write(m_Buffer->contents(), data);
            didModifyrange();
        }
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

	void MetalBuffer::setData(void* buffer, uint32_t size, uint32_t offset) {
		memcpy(m_Buffer->contents(), buffer, size);
	}

    void* MetalBuffer::getBuffer() const { 
        return m_Buffer;
    }

    void MetalBuffer::didModifyrange() {
        m_Buffer->didModifyRange(NS::Range::Make(0, m_Buffer->length()));
    }
	
}
