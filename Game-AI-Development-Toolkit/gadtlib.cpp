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

#include "gadtlib.h"

namespace gadt
{
	//a golbal color ostream 
	console::costream ccout(std::cout);

	namespace console
	{
		//change_color
		std::string costream::change_color(color::Color color)
		{
#ifndef GADT_UNIX
			HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(handle, color);
			return std::string("");
#else
			static string color_str[16] =
			{
				string(""),
				string("\e[0;40;34m"),		//deep_blue = 1,
				string("\e[0;40;32m"),		//deep_green = 2,
				string("\e[0;40;36m"),		//deep_cyan = 3,
				string("\e[0;40;31m"),		//brown = 4,
				string("\e[0;40;35m"),		//purple = 5,
				string("\e[0;40;33m"),		//deep_yellow = 6,
				string("\e[0m"),			//deep_white = 7,
				string("\e[0;40;37m"),		//gray = 8,
				string("\e[0;40;34m"),		//blue = 9,
				string("\e[0;40;32m"),		//green = 10,
				string("\e[0;40;36m"),		//cyan = 11,
				string("\e[0;40;31m"),		//red = 12,
				string("\e[0;40;35m"),		//pink = 13,
				string("\e[0;40;33m"),		//yellow = 14,
				string("\e[0;40;37m")		//white = 15
											//string("\e[0;40;37m")		//white = 15
			};
			return color_str[color];
#endif
		}

		//printf in costream.
		void costream::print(std::string str, color::Color color)
		{
			color::Color temp_color = _color;
			ccout << color << str << temp_color;
		}
		
		void ShowError(std::string reason)
		{
			std::cout << std::endl;
			Cprintf(">> ERROR: ", color::red);
			Cprintf(reason, color::white);
			std::cout << std::endl << std::endl;
		}

		void Cprintf(std::string tex, color::Color color)
		{
			ccout.print(tex, color);
		}

		void ShowMessage(std::string message, bool show_MSG)
		{
			std::cout << ">> ";
			if (show_MSG)
			{
				Cprintf("MSG: ", color::deep_green);
			}
			Cprintf(message, color::green);
			std::cout << std::endl << std::endl;
		}

		void WarningCheck(bool condition, std::string reason, std::string file, int line, std::string function)
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

		void SystemPause()
		{
#ifdef GADT_UNIX
			cout << "Press ENTER to continue." << endl;
			while (!getchar());
#else
			system("pause");
#endif
		}
	}
}

std::string gadt::timer::TimeString()
{
	time_t t = time(NULL);
	char buf[128] = { 0 };
#ifndef GADT_UNIX
	tm local;
	localtime_s(&local, &t);
	strftime(buf, 64, "%Y.%m.%d-%H:%M:%S", &local);
	return std::string(buf);
#else
	tm* local;
	local = localtime(&t);
	strftime(buf, 64, "%Y.%m.%d-%H:%M:%S", local);
	return std::string(buf);
#endif
}

clock_t gadt::timer::GetClock()
{
	return clock();
}

double gadt::timer::GetTimeDifference(const clock_t & start)
{
	return (double)(clock() - start) / CLOCKS_PER_SEC;
}
