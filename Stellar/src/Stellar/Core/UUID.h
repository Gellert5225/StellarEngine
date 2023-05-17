#pragma once
#if defined (_WIN64)
#include <xhash>
#endif

namespace Stellar {
	class UUID {
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID& other);

		operator uint64_t () { return m_UUID; }
		operator const uint64_t () const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};
		
}

namespace std {

	template <>
	struct hash<Stellar::UUID> {
		std::size_t operator()(const Stellar::UUID& uuid) const {
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}