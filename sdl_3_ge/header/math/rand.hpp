#pragma once
#include <random>
#include <algorithm>
#include <memory>
#include <cstdlib>

#include "../concepts.hpp"
namespace SKC::GE {
	template<typename T> 
	concept uni_rand_bitgen = std::uniform_random_bit_generator<T>;

	template<
		typename T,
		template <typename t, typename aloc = std::allocator<t>> 
			typename container_,
		 uni_rand_bitgen random_src = std::mt19937
	>
		requires concepts::random_access_container<container_,T> 
		
		T choose_from(container_<T> container) noexcept {
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
		catch (...) {
			//NOTE(skc): 
			//if this is reached there's a bug in the STL 
			//or you've invoked UB somewhere. 
			//Because index must be in the range [0,size)
			//also I am nearly positive doing this is the same as just not
			//catching the throw in the first place but this is much better because 
			//it spells out my intent. Also I can change this to an exit. 

			//TODO(skc) implement error printing.
			
			std::abort(); 
		}
		return  result; 
	}

	template<concepts::number T,
		uni_rand_bitgen random_src = std::mt19937>
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
