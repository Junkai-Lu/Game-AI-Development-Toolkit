/*
* gadt lib include some basic function of Game Ai Development Toolkit.
*
* version: 2017/1/17
* copyright: Junkai Lu
* email: Junkai-Lu@outlook.com
*/

#include "gadtlib.h"

namespace gadt
{
	namespace console
	{
		//colorful print
		void Cprintf(std::string tex, Color color)
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