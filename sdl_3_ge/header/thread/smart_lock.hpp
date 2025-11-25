#pragma once
#include <mutex> 
namespace SKC::threading {
	template <typename T, typename mutex = std::mutex> 
	class smart_lock {
		T m_data;
		mutex m_mutex;
	public:
		smart_lock() = default;
		explicit smart_lock(const T& data) : m_data(data) {}
		T get() {
			std::lock_guard lock{ m_mutex };
			return m_data;
		}

		void set(const T& data) {
			std::lock_guard lock{ m_mutex };
			m_data = data;
		}
		void operator = (const T& data) {
			set(data);
		}
		operator T() {
			return get();
		}
		T operator *() {
			return get();
		}
		bool operator == (const T& other) {
			return get() == other;
		}
		bool operator == (const smart_lock<T, mutex>& other) {
			return this == &other;
		}
	};
}