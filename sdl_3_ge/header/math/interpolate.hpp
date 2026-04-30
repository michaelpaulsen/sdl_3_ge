#pragma once
#include <concepts>
#include <functional>
#include <type_traits>
#include <numbers>
namespace SKC::Math {
	template<std::floating_point T>
	using easing_functor_t = std::function<T(T)>; 
	
	namespace interp_functors {
#define PRIV_INTERP_FUNCTOR_SIG [](std::floating_point auto t) -> decltype(t)
		constinit float PI_HALFSf =  std::numbers::pi_v<float>/2.f; 
		constinit float PI_HALFS  =  std::numbers::pi/2.; 

		auto linear = PRIV_INTERP_FUNCTOR_SIG {
			if (t < 0) return 0;
			if (t > 1) return 1;
			return t;
		};
		auto squared = PRIV_INTERP_FUNCTOR_SIG{
			if (t < 0) return 0;
			if (t > 1) return 1;
			return t * t;
		};
		auto sin_interp = PRIV_INTERP_FUNCTOR_SIG{
			if (t < 0) return 0;
			if (t > 1) return 1;
			if constexpr (std::is_same_v<decltype(t), float>) {
				return sinf(PI_HALFSf * t);

			}
			else {
				return sin(PI_HALFS * t);
			}
		};
		
#undef PRIV_INTERP_FUNCTOR_SIG
	}




	template<std::floating_point T, std::floating_point Y = T> 
	T interpolate(
		T value1,
		T value2,
		Y percent,
		easing_functor_t<T> ease_in,
		easing_functor_t<T> ease_out
	) {
		auto t1 = ease_in(1 - percent) * value1;
		auto t2 = ease_out(percent) * value2; 
		return t1 + t2; 
	}

	template<std::floating_point T, std::floating_point Y = T>
	T interpolate(
		T value1,
		T value2,
		Y percent,
		easing_functor_t<T> ease
	) {
		return interpolate(value1, value2, percent, ease, ease); 

	}
	template<std::floating_point T, std::floating_point Y = T>
	T lerp(
		T value1,
		T value2,
		Y percent
	) {
		return interpolate(value1, value2, interp_functors::linear);
	}
}