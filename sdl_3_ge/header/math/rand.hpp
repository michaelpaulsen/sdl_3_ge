#pragma once
#include <random>
#include <algorithm>
#include <optional>
#include <memory>

#include "../concepts.hpp"
namespace SKC::GE {
	
	template<
		typename T,
		template <typename t, typename aloc = std::allocator<t>> 
			typename container_,
		std::uniform_random_bit_generator random_src = std::mt19937
	>
		requires concepts::random_access_container<container_,T> 
		
	std::optional<T> choose_from(container_<T> container) noexcept {
		random_src generator(std::random_device{}());
		std::uniform_int_distribution<size_t> distribution(0, container.size() - 1);
		auto index = distribution(generator);
		if (container.size() == 0) {
			std::println("[RNG choose_from] empty container");
			return {}; 
		}
		T result{};
		try {
			result = container.at(index);
		}
		catch (const std::out_of_range& e) {
			std::println("[RNG choose_from] out of range access at index {}", index);
			std::println("[RNG choose_from] returning last value", index);
			return {}; 
		}
		catch (...) {
			std::println("[RNG choose_from] unknown error accessing index {}", index);
			return {}; 
		}
		return  result; 

	}

	template<concepts::number T,
		std::uniform_random_bit_generator random_src = std::mt19937>
	class RNG {
		random_src m_generator;
	public:
		T min = 0;
		T max = 6;
		RNG() : m_generator(std::random_device{}()) {}
		RNG(uint32_t seed) : m_generator(seed) {}

		T get_random() {
			if constexpr (std::is_integral_v<T>) {
				std::uniform_int_distribution<T> distribution(min, max);
				return distribution(m_generator);
			}
			else if constexpr (std::is_floating_point_v<T>) {
				std::uniform_real_distribution<T> distribution(min, max);
				return distribution(m_generator);
			}
		}
		T operator()() {
			return get_random(); 
		}
	};
}
