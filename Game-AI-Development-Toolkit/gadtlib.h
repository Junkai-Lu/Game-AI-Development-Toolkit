/*
* gadt lib include some basic function of Game Ai Development Toolkit.
*
*/

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

using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;
using std::ifstream;
using std::string;
using std::vector;
using std::map;
using std::stringstream;
using std::function;
using std::shared_ptr;

namespace gadt
{
	namespace console
	{
		//console color type
		namespace color
		{
			enum Color
			{
				deep_blue = 1,
				deep_green = 2,
				deep_cyan = 3,
				brown = 4,
				purple = 5,
				deep_yellow = 6,
				deep_white = 7,
				gray = 8,
				blue = 9,
				green = 10,
				cyan = 11,
				red = 12,
				pink = 13,
				yellow = 14,
				white = 15
			};
		}
		typedef color::Color Color;

		//colorful print.
		void Cprintf(std::string tex, Color color);

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

		inline void SystemPause()
		{
#ifdef GADT_UNIX
			cout << "Press ENTER to continue." << endl;
			while (!getchar());
#else
			system("pause");
#endif
		}

		//show error in terminal.
		inline void ShowError(std::string reason)
		{
			std::cout << std::endl;
			Cprintf(">> ERROR: ", color::red);
			Cprintf(reason, color::white);
			std::cout << std::endl << std::endl;
		}

		//show message in terminal.
		inline void ShowMessage(std::string message, bool show_MSG = true)
		{
			std::cout << ">> ";
			if (show_MSG)
			{
				Cprintf("MSG: ", color::deep_green);
			}
			Cprintf(message, color::green);
			std::cout << std::endl << std::endl;
		}

		//if 'condition' is true that report detail.
		inline void WarningCheck(bool condition, std::string reason, std::string file, int line, std::string function)
		{
			if (condition)
			{
				std::cout << std::endl << std::endl;
				Cprintf(">> WARNING: ", color::purple);
				Cprintf(reason, color::red);
				std::cout << std::endl;
				Cprintf("[File]: " + file, color::gray);
				std::cout << std::endl;
				Cprintf("[Line]: " + I2S(line), color::gray);
				std::cout << std::endl;
				Cprintf("[Func]: " + function, color::gray);
				std::cout << std::endl;
				console::SystemPause();
			}
		}
	}

	namespace timer
	{
		std::string TimeString();
		clock_t GetClock();
		double GetTimeDifference(const clock_t& start);
	}
}
