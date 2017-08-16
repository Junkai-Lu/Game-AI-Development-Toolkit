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

using std::cout;
using std::endl;
using std::string;

namespace gadt
{
	namespace console
	{
		//a golbal color ostream 
		costream ccout(std::cout);

		//global color.
		ConsoleColor costream::_global_color = DEFAULT;

		//change_color
		void costream::change_color(ConsoleColor color)
		{
#ifdef __GADT_MSVC
			HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
			SetConsoleTextAttribute(handle, color);
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
			std::cout << color_str[color];
#endif
		}
		
		//show error
		void ShowError(std::string reason)
		{
			//std::cout << std::endl;
			Cprintf(">> ERROR ", PURPLE);
			std::cout << ": ";
			Cprintf(reason, RED);
			std::cout  << std::endl;
		}

		//show message
		void ShowMessage(std::string message, bool show_msg)
		{
			std::cout << ">> ";
			if (show_msg)
			{
				Cprintf("MSG: ", DEEP_GREEN);
			}
			Cprintf(message, GREEN);
			std::cout << std::endl << std::endl;
		}

		//warning check.
		void WarningCheck(bool condition, std::string reason, std::string file, int line, std::string function)
		{
			if (condition)
			{
				std::cout << std::endl << std::endl;
				Cprintf(">> WARNING: ", PURPLE);
				Cprintf(reason, RED);
				std::cout << std::endl;
				Cprintf("[File]: " + file, GRAY);
				std::cout << std::endl;
				Cprintf("[Line]: " + IntergerToString(line), GRAY);
				std::cout << std::endl;
				Cprintf("[Func]: " + function, GRAY);
				std::cout << std::endl;
				console::SystemPause();
			}
		}

		//system pause
		void SystemPause()
		{
#ifdef __GADT_GNUC
			cout << "Press ENTER to continue." << endl;
			while (!getchar());
#elif defined(__GADT_MSVC)
			system("pause");
#endif
		}
		
		//clear system screen
		void SystemClear()
		{
#ifdef __GADT_GNUC
			system("clear");
#elif defined(__GADT_MSVC)
			system("cls");
#endif
		}
	}

	namespace timer
	{
		std::string TimeString(std::string format)
		{
			time_t t = time(NULL);
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

		clock_t GetClock()
		{
			return clock();
		}

		double GetTimeDifference(const clock_t & start)
		{
			return (double)(clock() - start) / CLOCKS_PER_SEC;
		}

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

	namespace file
	{
		bool DirExist(std::string path)
		{
#ifdef __GADT_GNUC
			return (access(path.c_str(), 0) != -1);
#elif defined(__GADT_MSVC)
			return (_access(path.c_str(), 0) != -1);
#endif
		}

		bool MakeDir(std::string path)
		{
			if (DirExist(path))
			{
				return true;
			}
#ifdef __GADT_GNUC
			return mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1;
			//return false;
#elif defined(__GADT_MSVC)
			return _mkdir(path.c_str()) != -1;
#endif
		}

		bool RemoveDir(std::string path)
		{
			if (!DirExist(path))
			{
				return false;
			}
#ifdef __GADT_GNUC
			return rmdir(path.c_str()) != -1;
			//return false;
#elif defined(__GADT_MSVC)
			return _rmdir(path.c_str()) != -1;
#endif
		}
	}

}