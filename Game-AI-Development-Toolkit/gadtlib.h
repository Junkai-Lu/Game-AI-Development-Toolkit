/*
* gadt lib include some basic function of Game Ai Development Toolkit.
*
* version: 2017/2/10
* copyright: Junkai Lu
* email: Junkai-Lu@outlook.com
*/


// exclude unsupported compilers
#if defined(__clang__)
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
#define GADT_WARNING_CHECK(warning_condition, reason) gadt::console::WarningCheck(warning_condition, reason, __FILE__, __LINE__, __FUNCTION__)

#ifndef GADT_UNIX
	#include <SDKDDKVer.h>
	#include <Windows.h>
#endif

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <tchar.h>
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
		void Cprintf(std::string tex, color::Color color);
		inline std::string B2S(bool b)
		{
			if (b)
			{
				return "true";
			}
			return "false";
		}
		inline std::string I2S(size_t i)
		{
			std::stringstream ss;
			ss << i;
			return ss.str();
		}
		inline void ShowError(std::string reason)
		{
			std::cout << std::endl;
			Cprintf(">> ERROR: ", color::red);
			Cprintf(reason, color::white);
			std::cout << std::endl << std::endl;
		}
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
		inline void WarningCheck(bool condition, std::string reason, std::string file, int line, std::string function)
		{
#ifdef GADT_WARNING
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
				system("pause");
			}
#endif
		}
	}

	namespace timer
	{
		std::string TimeString();
		clock_t GetClock();
		double GetTimeDifference(const clock_t& start);
	}
}
