#pragma once

#include <stdint.h>

namespace Stellar {
	class STLR_Base {
	public:
		void incrementRefCount() const { m_RefCount++; }

		void decrementRefCount() const { m_RefCount--; }

		uint32_t getRefCount() const { return m_RefCount; }
	private:
		mutable uint32_t m_RefCount = 0;
	};

	template<typename T>
	class STLR_Ptr {
	public:
		STLR_Ptr() : m_Instance(nullptr) {};
		STLR_Ptr(std::nullptr_t ptr) : m_Instance(nullptr) {}
		STLR_Ptr(T* instance) : m_Instance(instance) {
			static_assert(std::is_base_of<STLR_Base, T>::value, "Class is not extended from STLR_Base!");

			incrementRef();
		}

		template<typename T2>
		STLR_Ptr(const STLR_Ptr<T2>& other) {
			m_Instance = (T*)other.m_Instance;
			incrementRef();
		}

		template<typename T2>
		STLR_Ptr(STLR_Ptr<T2>&& other) {
			m_Instance = (T*)other.m_Instance;
			other.m_Instance = nullptr;
		}

		~STLR_Ptr() {
			decrementRef();
		}

		STLR_Ptr(const STLR_Ptr<T>& other) : m_Instance(other.m_Instance) {
			incrementRef();
		}

		STLR_Ptr& operator=(std::nullptr_t) {
			decrementRef();
			m_Instance = nullptr;
			return *this;
		}

		STLR_Ptr& operator=(const STLR_Ptr<T>& other) {
			other.incrementRef();
			decrementRef();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		STLR_Ptr& operator=(const STLR_Ptr<T2>& other) {
			other.incrementRef();
			decrementRef();

			m_Instance = other.m_Instance;
			return *this;
		}

		template<typename T2>
		STLR_Ptr& operator=(STLR_Ptr<T2>&& other) {
			decrementRef();

			m_Instance = other.m_Instance;
			other.m_Instance = nullptr;
			return *this;
		}

		operator bool() { return m_Instance != nullptr; }
		operator bool() const { return m_Instance != nullptr; }

		T* operator->() { return m_Instance; }
		const T* operator->() const { return m_Instance; }

		T& operator*() { return *m_Instance; }
		const T& operator*() const { return *m_Instance; }

		T* raw() { return  m_Instance; }
		const T* raw() const { return  m_Instance; }

		void Reset(T* instance = nullptr) {
			decrementRef();
			m_Instance = instance;
		}

		template<typename T2>
		STLR_Ptr<T2> As() const {
			return STLR_Ptr<T2>(*this);
		}

		template<typename... Args>
		static STLR_Ptr<T> Create(Args&&... args) {
			return STLR_Ptr<T>(new T(std::forward<Args>(args)...));
		}

		bool operator==(const STLR_Ptr<T>& other) const {
			return m_Instance == other.m_Instance;
		}

		bool operator!=(const STLR_Ptr<T>& other) const {
			return !(*this == other);
		}

		bool EqualsObject(const STLR_Ptr<T>& other) {
			if (!m_Instance || !other.m_Instance)
				return false;
			
			return *m_Instance == *other.m_Instance;
		}
	private:
		void incrementRef() const {
			if (m_Instance)
				m_Instance->incrementRefCount();
		}
		void decrementRef() const {
			if (m_Instance) {
				m_Instance->decrementRefCount();
				if (m_Instance->getRefCount() == 0)
					delete m_Instance;
			}
		}

		template<class T2>
		friend class STLR_Ptr;
		T* m_Instance;
	};
}