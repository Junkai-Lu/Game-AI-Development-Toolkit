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

#include "gadtlib.h"

using std::cout;
using std::endl;
using std::string;

namespace gadt
{
	namespace console
	{
		//set global console color.
		ConsoleColor costream::_current_color = ConsoleColor::Default;

		//change_color
		void costream::change_color(ConsoleColor color)
		{
			_current_color = color;
#ifdef __GADT_MSVC
			HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(handle, static_cast<int8_t>(color));
#elif defined(__GADT_GNUC)
			static std::string color_str[16] =
			{
				string(""),
				string("\e[0;40;34m"),		//deep_blue = 1,
				string("\e[0;40;32m"),		//deep_green = 2,
				string("\e[0;40;36m"),		//deep_cyan = 3,
				string("\e[0;40;31m"),		//brown = 4,
				string("\e[0;40;35m"),		//purple = 5,
				string("\e[0;40;33m"),		//deep_yellow = 6,
				string("\e[0m"),			//deep_white = 7,
				string("\e[1;40;30m"),		//gray = 8,
				string("\e[1;40;34m"),		//blue = 9,
				string("\e[1;40;32m"),		//green = 10,
				string("\e[1;40;36m"),		//cyan = 11,
				string("\e[1;40;31m"),		//red = 12,
				string("\e[1;40;35m"),		//pink = 13,
				string("\e[1;40;33m"),		//yellow = 14,
				string("\e[0;40;37m")		//white = 15
											//string("\e[0;40;37m")		//white = 15
			};
			std::cout << color_str[static_cast<size_t>(color)];
#endif
		}

		//get user confirm(Y/N or y/n)
		bool GetUserConfirm(std::string tip)
		{
			for (;;)
			{
				std::cout << ">> ";
				Cprintf(tip + "(y/n):", ConsoleColor::Gray);
				std::string input = GetInput("");
				if (input == "Y" || input == "y")
					return true;
				if (input == "N" || input == "n")
					return false;
				PrintError("invaild input, please input 'y' or 'n'.");
			}
			return false;
		}

		//show error
		void PrintError(std::string reason)
		{
			//std::cout << std::endl;
			Cprintf(">> ERROR", ConsoleColor::Purple);
			std::cout << ": ";
			Cprintf(reason, ConsoleColor::Red);
			PrintEndLine<2>();
		}

		//show message
		void PrintMessage(std::string message)
		{
			std::cout << ">> ";
			Cprintf(message, ConsoleColor::White);
			PrintEndLine<2>();
		}

		//warning check.
		void WarningCheck(bool condition, std::string reason, std::string file, int line, std::string function)
		{
			if (condition)
			{
				PrintEndLine<2>();
				Cprintf(">> WARNING: ", ConsoleColor::Purple);
				Cprintf(reason, ConsoleColor::Red);
				PrintEndLine();
				Cprintf("[File]: " + file, ConsoleColor::Gray);
				PrintEndLine();
				Cprintf("[Line]: " + ToString(line), ConsoleColor::Gray);
				PrintEndLine();
				Cprintf("[Func]: " + function, ConsoleColor::Gray);
				PrintEndLine();
				console::SystemPause();
			}
		}

		//pause.
		void SystemPause()
		{
#ifdef __GADT_GNUC
			cout << "Press ENTER to continue." << endl;
			while (!getchar());
#elif defined(__GADT_MSVC)
			system("pause");
#endif
		}

		//clear screen
		void SystemClear()
		{
#ifdef __GADT_GNUC
			std::cout << "\33[2J";
#elif defined(__GADT_MSVC)
			system("cls");
#endif
		}
	}

	namespace timer
	{
		//获得时间
		std::string TimePoint::get_string(std::string format) const
		{
			time_t t = _time;
			char buf[128] = { 0 };
#ifdef __GADT_MSVC
			tm local;
			localtime_s(&local, &t);
			strftime(buf, 64, format.c_str(), &local);
			return std::string(buf);
#else
			tm* local;
			local = localtime(&t);
			strftime(buf, 64, format.c_str(), local);
			return std::string(buf);
#endif
		}

	}

	namespace os
	{
		//get number of processors.
		size_t SysNumberOfProcessors()
		{
#ifdef __GADT_MSVC
			SYSTEM_INFO sysInfo;
			GetSystemInfo(&sysInfo);
			return (size_t)sysInfo.dwNumberOfProcessors;
#else
			return (size_t)sysconf(_SC_NPROCS_ONLN));;
#endif
		}
	}

	namespace func
	{
		size_t GetManhattanDistance(Point fir, Point sec)
		{
			return abs(fir.x - sec.x) + abs(fir.y - sec.y);
		}
	}
}


