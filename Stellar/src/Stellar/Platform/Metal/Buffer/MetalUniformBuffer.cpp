#include "stlrpch.h"

#include "MetalUniformBuffer.h"

#include "Stellar/Renderer/Uniforms.h"

#include "Stellar/Platform/Metal/Device/MetalDevice.h"
#include "Stellar/Core/Log.h"

namespace Stellar {
	MetalUniformBuffer::MetalUniformBuffer(uint32_t size, uint32_t binding) {
		auto device = MetalDevice::GetInstance()->getDevice();
		m_Buffer = device->newBuffer(size, MTL::ResourceStorageModeManaged);
	}

	MetalUniformBuffer::~MetalUniformBuffer() {
        m_Buffer->release();
    }

	void MetalUniformBuffer::setData(const void* data, uint32_t size, uint32_t offset) {
		reinterpret_cast<GlobalUniforms*>(m_Buffer->contents())->viewProjection = *(const glm::mat4*)data;
        didModifyrange();
	}

	void MetalUniformBuffer::didModifyrange() {
		 m_Buffer->didModifyRange(NS::Range::Make(0, m_Buffer->length()));
	}

	MTL::Buffer* MetalUniformBuffer::getBuffer() {
		return m_Buffer;
	}

}