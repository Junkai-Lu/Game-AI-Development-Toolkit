/* Copyright (c) 2017 Junkai Lu <junkai-lu@outlook.com>.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

// exclude unsupported compilers and define some marco by compiler.
#ifdef _MSC_VER
	#include <SDKDDKVer.h>
	#include <Windows.h>
	#include <tchar.h>
#elif defined(__clang__)
	#define GADT_UNIX
	#if (__clang_major__ * 10000 + __clang_minor__ * 100 + __clang_patchlevel__) < 30400
		#error "unsupported Clang version"
	#endif
#elif defined(__GNUC__)
	#define GADT_UNIX
	#if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40900
		#error "unsupported GCC version"
	#endif
#endif

//open debug-info option to include extra info , this would lead to little performance penalties.
#define GADT_DEBUG_INFO

//warning option allow the functions in GADT execute parameter check and report wrong parameters.
#define GADT_WARNING

//a marco use for parameters check.
#ifdef GADT_WARNING
	#define GADT_WARNING_CHECK(warning_condition, reason) gadt::console::WarningCheck(warning_condition, reason, __FILE__, __LINE__, __FUNCTION__);
#else
	#define GADT_WARNING_CHECK(warning_condition, reason)
#endif

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <set>

#pragma once

namespace gadt
{
	namespace console
	{
		//console color type
		enum ConsoleColor
		{
			DEFAULT = 7,
			DEEP_BLUE = 1,
			DEEP_GREEN = 2,
			DEEP_CYAN = 3,
			BROWN = 4,
			PURPLE = 5,
			DEEP_YELLOW = 6,
			DEEP_WHITE = 7,
			GRAY = 8,
			BLUE = 9,
			GREEN = 10,
			CYAN = 11,
			RED = 12,
			PINK = 13,
			YELLOW = 14,
			WHITE = 15
		};

		//color ostream.
		class costream
		{
		private:
			std::ostream& _os;
			ConsoleColor _color;

			//change colors by setting using winapi in windows or output string in linux.
			static std::string change_color(ConsoleColor color);
		public:
			costream(std::ostream& os) : _os(os),_color(DEFAULT) {}
			costream& operator<<(ConsoleColor color)
			{
				_color = color;
				_os << change_color(color);
				return *this;
			}

			template<typename datatype>
			inline costream& operator<<(datatype d)
			{
				_os << d;
				return *this;
			}

			template <typename t_data>
			void print(t_data data, ConsoleColor color);
		};
		
		//bool to string. that can be replaced by '<< boolalpha'
		inline std::string B2S(bool b)
		{
			if (b)
			{
				return "true";
			}
			return "false";
		}

		//interger to string.
		inline std::string I2S(size_t i)
		{
			std::stringstream ss;
			ss << i;
			return ss.str();
		}

		//colorful print.
		void Cprintf(std::string	data,	ConsoleColor color);
		void Cprintf(double			data,	ConsoleColor color);
		void Cprintf(int			data,	ConsoleColor color);
		void Cprintf(char			data,	ConsoleColor color);

		//show error in terminal.
		void ShowError(std::string reason);

		//show message in terminal.
		void ShowMessage(std::string message, bool show_msg = true);

		//if 'condition' is true that report detail.
		void WarningCheck(bool condition, std::string reason, std::string file, int line, std::string function);

		//system pause.
		void SystemPause();
	}

	//a golbal color ostream 
	extern console::costream ccout;

	namespace timer
	{
		std::string TimeString();
		clock_t GetClock();
		double GetTimeDifference(const clock_t& start);
	}
}

