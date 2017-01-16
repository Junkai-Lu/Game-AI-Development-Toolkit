/*
* gadt lib include some basic function of Game Ai Development Toolkit.
*
* version: 2017/1/16
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
#ifndef GADT_UNIX
			time_t t;
			tm local;
			char buf[128] = { 0 };
			t = time(NULL);
			localtime_s(&local, &t);
			strftime(buf, 64, "%Y.%m.%d-%H:%M:%S", &local);
			return std::string(buf);
#else
			time_t t;  //秒时间  
			tm* local; //本地时间   
			char buf[128] = { 0 };
			t = time(NULL); //获取目前秒时间
			local = localtime(&t); //转为本地时间  
			strftime(buf, 64, "%Y.%m.%d-%H:%M:%S", local);
			return std::string(buf);
#endif
		}
		
		//create timepoint.
		clock_t GetTimePoint()
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