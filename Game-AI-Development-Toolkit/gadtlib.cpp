/*
* gadt lib include some basic function of Game Ai Development Toolkit.
*
* version: 2017/1/13
* copyright: Junkai Lu
* email: Junkai-Lu@outlook.com
*/

#include "stdafx.h"
#include "gadtlib.h"

namespace gadt
{
	namespace console
	{
		//colorful print
		void Cprintf(std::string tex, console::color::Color color)
		{
#ifndef GADT_GCC
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
}