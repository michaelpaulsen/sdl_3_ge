#pragma once
#include <concepts>
namespace SKC::concepts {
	//typing concepts.
	template <typename T> concept isNumber = std::is_integral_v<T> || std::is_floating_point_v<T>;
	template <typename T> concept isString = std::is_convertible_v<T, std::string>;
	// above is std::string, [cv qualifided] char * and [cv qualifided] char[]; 

	template <typename T> concept isAlNum  = isNumber<T> || isString<T>; 

	//interface for the printable 
	template <typename T> concept parsable_cimple = requires(T t) { { t.ToString() } -> isString; };
	template <typename T> concept parsable_imple  = requires(T t) { { t.toString() } -> isString; };
	template <typename T> concept parsable        = parsable_cimple<T> || parsable_imple<T>;
	template <typename T> concept printable       = parsable<T> || isAlNum<T>;

}