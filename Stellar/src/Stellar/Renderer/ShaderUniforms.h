#pragma once

#include "Stellar/Core/Log.h"

#include <string>
#include <vector>

namespace Stellar {

	enum class ShaderDomain {
		None = 0, Vertex = 0, Pixel = 1
	};

	class ShaderResourceDeclaration {
	public:
		ShaderResourceDeclaration() = default;
		ShaderResourceDeclaration(const std::string& name, uint32_t resourceRegister, uint32_t count)
			: m_Name(name), m_Register(resourceRegister), m_Count(count) {}

		virtual const std::string& getName() const { return m_Name; }
		virtual uint32_t getRegister() const { return m_Register; }
		virtual uint32_t getCount() const { return m_Count; }
	private:
		std::string m_Name;
		uint32_t m_Register = 0;
		uint32_t m_Count = 0;
	};

	typedef std::vector<ShaderResourceDeclaration*> ShaderResourceList;

}
