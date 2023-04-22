#pragma once

#include "Stellar/Core/Core.h"
#include "Stellar/Core/Log.h"
#include "Stellar/Core/STLRBase.h"
#include <glm/glm.hpp>

namespace Stellar {
	enum class ShaderDataType {
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type) {
		switch (type) {
			case ShaderDataType::Float:    return 4;
			case ShaderDataType::Float2:   return 4 * 2;
			case ShaderDataType::Float3:   return 4 * 3;
			case ShaderDataType::Float4:   return 4 * 4;
			case ShaderDataType::Mat3:     return 4 * 3 * 3;
			case ShaderDataType::Mat4:     return 4 * 4 * 4;
			case ShaderDataType::Int:      return 4;
			case ShaderDataType::Int2:     return 4 * 2;
			case ShaderDataType::Int3:     return 4 * 3;
			case ShaderDataType::Int4:     return 4 * 4;
			case ShaderDataType::Bool:     return 1;
		}

		STLR_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	enum class BufferType {
		Vertex,
		Index,
		Uniform
	};

	struct Vertex {
		glm::vec2 position;
		glm::vec2 texCoord;
		float tilingFactor;
	};

	struct VertexElement {
		std::string name;
		ShaderDataType type;
		uint32_t size;
		uint32_t offset;

		VertexElement() = default;
		VertexElement(const std::string& name, ShaderDataType type)
			: name(name), type(type), size(ShaderDataTypeSize(type)), offset(0) {}
	};

	class Buffer : public STLR_Base {
	public:
		virtual ~Buffer() = default;

		virtual void copy(const Buffer& dst) = 0;
		[[nodiscard]] virtual void* getBuffer() const = 0;

		static STLR_Ptr<Buffer> Create(BufferType type, uint64_t size, const void* data = nullptr);

		virtual void map(void** data) = 0;
		virtual void unMap() = 0;
		virtual void write(void* dst, const void* src) = 0;
	protected:
		explicit Buffer(uint64_t size) : m_Size(size) {}
		uint64_t m_Size = 0;
	};

	class VertexBufferLayout {
	public:
		VertexBufferLayout() {}
		VertexBufferLayout(const std::initializer_list<VertexElement>& elements) :m_Elements(elements) {
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements) {
				element.offset = offset;
				offset += element.size;
				m_Stride += element.size;
			}
		}

		uint32_t getStride() const { return m_Stride; }
		uint32_t getElementCount() const { return (uint32_t)m_Elements.size(); }

		// iterators
		std::vector<VertexElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<VertexElement>::iterator end() { return m_Elements.end(); }
		std::vector<VertexElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<VertexElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		std::vector<VertexElement> m_Elements;
		uint32_t m_Stride = 0;
	};
}