#pragma once
#include <mutex> 
namespace SKC::threading {
	template <typename T> 
	class smart_lock {
		using mutex = std::mutex; 
		T m_data;
		mutex m_mutex;
		bool locked = false; 
	public:
		smart_lock() = default;
		explicit smart_lock(const T& data) : m_data(data) {}
		~smart_lock() {
			if (locked) {
				m_mutex.unlock(); 
			}
		}
		T get_copy() {
			std::lock_guard lock{ m_mutex };
			return m_data;
		}
		void lock() {
			locked = true; 
			m_mutex.lock();

		}
		void unlock() {
			if (locked) {
				m_mutex.unlock();
			}
			locked = false; 
		}
		[[nodiscard]] T& get() {
			if (!locked) {
				exit(-1); 
			}
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
			return get_copy();
		}
		T operator *() {
			return get_copy();
		}
		bool operator == (const T& other) {
			return get_copy() == other;
		}
		bool operator == (const smart_lock<T>& other) {
			return this == &other;
		}
	};
}