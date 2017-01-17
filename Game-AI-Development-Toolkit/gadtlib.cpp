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
		void Cprintf(std::string tex, console::color::Color color)
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
			std::cout << tex;
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