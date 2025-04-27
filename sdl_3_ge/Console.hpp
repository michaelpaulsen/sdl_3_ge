#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include "./concepts.hpp"
#define SKC_consoleVA template<concepts::printable printType, concepts::printable... printTypes>

namespace SKC{
	class Console
	{
		using color_t = unsigned char;
		static inline char esc = 27;
	public:
		Console() = default;
		~Console() = default;
		Console(Console&) = delete;
		Console(Console&&) = delete;
		void  operator =(Console&) = delete;
		void  operator =(Console&&) = delete;




		auto& SetFGColor(color_t r, color_t g, color_t b);
		auto& SetBGColor(color_t r, color_t g, color_t b);
		auto& NoBlink() {
			printf("%c[%dm", esc, 25);
			return *this;
		}
		auto& Blink();
		auto& Reset();
		auto& Clear();
		auto& Hide();
		auto& Move(int x, int y);
		auto& Up(int col = 1) {
			printf("\033[%dA", col);
			return *this;
		}
		auto& Down(int col = 1) {
			printf("\033[%dB", col);
			return *this;
		}
		auto& Left(int col = 1) {
			printf("\033[%dD", col);
			return *this;
		}
		auto& Right(int col = 1) {
			printf("\033[%dC", col);
			return *this;
		}
		auto& ClearLine(int mode = 2) {
			printf("\033[%dK", mode);
			return *this;
		}

		auto& Ok() { return *this; }
		auto& Warn() { return *this; }
		auto& Error() { return *this; }
		auto& Print() { return *this; }
		auto& Inform() { return *this; }

		auto& Okln() { return *this; }
		auto& Warnln() { return *this; }
		auto& Errorln() { return *this; }
		auto& Println() { return *this; }
		auto& Informln() { return *this; }

		SKC_consoleVA auto& Ok(printType msg, printTypes ...msgs);
		SKC_consoleVA auto& Okln(printType msg, printTypes ...msgs);

		SKC_consoleVA auto& Warn(printType msg, printTypes ...msgs);
		SKC_consoleVA auto& Warnln(printType msg, printTypes ...msgs);

		SKC_consoleVA auto& Print(printType msg1, printTypes... msg2);
		SKC_consoleVA auto& Println(printType msg1, printTypes... msg2);

		SKC_consoleVA auto& Error(printType msg, printTypes ...msgs);
		SKC_consoleVA auto& Errorln(printType msg, printTypes ...msgs);

		SKC_consoleVA auto& Inform(printType msg, printTypes ...msgs);
		SKC_consoleVA auto& Informln(printType msg, printTypes ...msgs);

		SKC_consoleVA auto& ErrorAndDie(int exitcode, printType msg, printTypes ...msgs);
		auto& ProgressBar(double min, double max, double val, int width);
	};

	auto& Console::SetFGColor(color_t r, color_t g, color_t b) {
		printf("%c[38;2;%d;%d;%dm", esc, r, g, b);
		return *this;
	}
	auto& Console::SetBGColor(color_t r, color_t g, color_t b) {
		printf("%c[48;2;%d;%d;%dm", esc, r, g, b);
		return *this;
	}


	auto& Console::Reset() {
		printf("%c[%cm", esc, 0);
		return *this;
	}
	auto& Console::Blink() {
		printf("%c[%dm", esc, 5);
		return *this;
	}
	auto& Console::Hide() {
		printf("%c[%cm", esc, 8);
		return *this;
	}
	auto& Console::Clear() {
		printf("%cc", esc);
		return *this;
	}

	auto& Console::Move(int x, int y) {
		printf("%c[%d;%dH", esc, y, x);
		return *this;
	}

	SKC_consoleVA auto& Console::Print(printType msg1, printTypes... msg2) {
		using t = decltype(msg1);
		if constexpr (concepts::parsable_cimple<t>) {
			// if the type has a ToString method call it
			// and the rest of the function doesn't even exist... 
			std::cout << msg1.ToString();
		}
		else if constexpr (concepts::parsable_imple<t>) {
			//else if it has a toString method call that
			std::cout << msg1.toString();
		}
		else {
			std::cout << msg1; // do nothing 
			//this should only be called for integral, floating point; and [c]strings
		}
		Print(msg2...);
		return *this;
	}
	SKC_consoleVA auto& Console::Println(printType msg1, printTypes... msg2) {
		Print(msg1, msg2..., '\n');
		return *this;
	}

	SKC_consoleVA auto& Console::Ok(printType msg1, printTypes... msg2) {
		SetBGColor(64, 255, 64);
		SetFGColor(128, 256, 128);
		Print(msg1, msg2...);
		return *this;

	}
	SKC_consoleVA auto& Console::Inform(printType msg1, printTypes... msg2) {
		SetBGColor(64, 128, 128);
		SetFGColor(128, 255, 255);
		Print(msg1, msg2...);
		return *this;

	}
	SKC_consoleVA auto& Console::Warn(printType msg1, printTypes... msg2) {
		SetBGColor(128, 128, 64);
		SetFGColor(256, 256, 128);
		Print(msg1, msg2...);
		return *this;

	}
	SKC_consoleVA auto& Console::Error(printType msg1, printTypes... msg2) {
		SetFGColor(255, 0, 0);
		Print(msg1, msg2...);
		return *this;
	}

	SKC_consoleVA auto& Console::Okln(printType msg1, printTypes... msg2) {
		Ok(msg1, msg2...);
		Reset();
		Print('\n');
		return *this;

	}
	SKC_consoleVA auto& Console::Informln(printType msg1, printTypes... msg2) {
		Inform(msg1, msg2...);
		Reset();
		Print('\n');
		return *this;

	}
	SKC_consoleVA auto& Console::Warnln(printType msg1, printTypes... msg2) {
		Warn(msg1, msg2...);
		Reset();
		Print('\n');
		return *this;

	}
	SKC_consoleVA auto& Console::Errorln(printType msg1, printTypes... msg2) {
		Error(msg1, msg2...);
		Reset();
		Print('\n');
		return *this;

	}
	SKC_consoleVA auto& Console::ErrorAndDie(int exitcode, printType msg1, printTypes... msg2) {
		Error(msg1, msg2...);
		Reset();
		exit(exitcode);
		return *this;
	}


	auto& Console::ProgressBar(double min, double max, double val, int width) {
		double range = max - min;
		double v = val - min;
		int hashes = (int)round((v / range) * width);
		Print('[', val, '/', max, "][");
		for (int x = 1; x < width; ++x) {
			if (x < hashes) {
				Print("#");
				continue;
			}
			Print('.');
		}
		Print("]\r");
		//ClearLine(0);
		return *this;
	}

}