#include <chrono>
namespace skc::chr {

	auto now_as_lt() {
		namespace chr = std::chrono;
		using sysclk = chr::system_clock;
		return chr::current_zone()->to_local(sysclk::now());
	}
	auto get_current_day() {
		namespace chr = std::chrono;
		using sysclk = chr::system_clock;
		return chr::floor<chr::days>(now_as_lt());
	}
}