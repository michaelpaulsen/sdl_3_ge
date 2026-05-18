#pragma once 
#include <numbers>
namespace SKC::Numbers {
	static constexpr inline double DEG_TO_RAD = (std::numbers::pi / 180.);
	static constexpr inline double RAD_TO_DEG = (180. / std::numbers::pi);
	static constexpr inline double        TAU = std::numbers::pi*2;

}