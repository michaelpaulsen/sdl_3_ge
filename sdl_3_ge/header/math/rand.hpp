#pragma once
#include <random>
#include <cstdint>
#include <type_traits>
#include <algorithm>

#include "../concepts.hpp"
namespace SKC::GE {


	template<concepts::number T, std::uniform_random_bit_generator random_src = std::mt19937>
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
