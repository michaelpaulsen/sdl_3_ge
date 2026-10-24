#pragma once
#include <mutex> 
namespace SKC::threading {
	template <typename T> 
	class owning_mutex {
		using mutex = std::mutex; 
		T m_data;
		mutex m_mutex;
		bool locked = false; 
	public:
		owning_mutex() = default;
		~owning_mutex() = default;
		
		explicit owning_mutex(const T& data) : m_data(data) {}
		bool try_lock() {
			return m_mutex.try_lock(); 
		}
		void lock() {
			m_mutex.lock();
		}
		T& get() {
			return m_data; 
		}
		void unlock() {
			m_mutex.unlock();
		}
		void set(const T& data) {
			std::lock_guard lock{ m_mutex };
			m_data = data;
		}
	};
}