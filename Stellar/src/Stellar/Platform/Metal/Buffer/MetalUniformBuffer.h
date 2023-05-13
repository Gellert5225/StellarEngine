#pragma once

#include "Stellar/Renderer/UniformBuffer.h"

#include "Stellar/Platform/Metal/MetalAPI.h"

namespace Stellar {
	class MetalUniformBuffer : public UniformBuffer {
	public:
		MetalUniformBuffer(uint32_t size, uint32_t binding);
		virtual ~MetalUniformBuffer();

		virtual void setData(const void* data, uint32_t size, uint32_t offset = 0) override;
		virtual uint32_t getBinding() const override { return m_Binding; }

		void didModifyrange();
		MTL::Buffer* getBuffer();
	private:
		MTL::Buffer* m_Buffer;

		uint32_t m_Size = 0;
		uint32_t m_Binding = 0;
		std::string m_Name;
	};
}