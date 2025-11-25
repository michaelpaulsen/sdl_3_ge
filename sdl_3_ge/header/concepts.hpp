#pragma once
#include <concepts>
namespace SKC {

	namespace concepts {
	//typing concepts.

		template <typename T> concept isNumber = !std::is_same_v<T, bool> && (std::is_integral_v<T> || std::is_floating_point_v<T>);
		template <typename T> concept isString = std::is_same_v<T, std::string>; 

		template <typename T> concept isAlNum = isNumber<T> || isString<T>;

		template <typename T> concept number = isNumber<T>;
		template <typename T> concept string = isString<T>;
		template <typename T> concept alnum = isAlNum<T>;
		template <typename T> concept boolalnum = alnum<T> || std::is_same_v<T, bool>;
	}
	namespace type_traits {
		template <typename T>
		struct is_number : std::bool_constant<concepts::isNumber<T>> {};
		template <typename T> constexpr bool is_number_v = is_number<T>::value;

}
}