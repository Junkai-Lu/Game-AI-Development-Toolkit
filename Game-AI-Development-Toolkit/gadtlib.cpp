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
	namespace console
	{
		//colorful print
		void Cprintf(std::string tex, color::Color color)
		{
#ifndef GADT_UNIX
			WORD colorOld;
			HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(handle, &csbi);
			colorOld = csbi.wAttributes;
			SetConsoleTextAttribute(handle, color);
			std::cout << tex;
			SetConsoleTextAttribute(handle, colorOld);
#else
			static string color_str[16] =
			{
				string(""),
				string("\e[1;40;34m"),		//deep_blue = 1,
				string("\e[1;40;32m"),		//deep_green = 2,
				string("\e[1;40;36m"),		//deep_cyan = 3,
				string("\e[1;40;31m"),		//brown = 4,
				string("\e[1;40;35m"),		//purple = 5,
				string("\e[1;40;33m"),		//deep_yellow = 6,
				string("\e[1;40;37m"),		//deep_white = 7,
				string("\e[1;40;37m"),		//gray = 8,
				string("\e[0;40;34m"),		//blue = 9,
				string("\e[0;40;32m"),		//green = 10,
				string("\e[0;40;36m"),		//cyan = 11,
				string("\e[0;40;31m"),		//red = 12,
				string("\e[0;40;35m"),		//pink = 13,
				string("\e[0;40;33m"),		//yellow = 14,
				string("\e[0;40;37m")		//white = 15
			};
			std::cout << color_str[color] << tex << "\e[0m";
			//std::cout << tex;
#endif
		}
	}

	namespace timer
	{
		//get string format of current time.
		std::string TimeString()
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

		//create timepoint.
		clock_t GetClock()
		{
			return clock();
		}

		//get time difference between current time and start time.
		double GetTimeDifference(const clock_t& start)
		{
			return (double)(clock() - start) / CLOCKS_PER_SEC;
		}

	}
}
