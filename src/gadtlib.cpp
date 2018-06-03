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
		ConsoleColor costream::_current_color = COLOR_DEFAULT;

		//change_color
		void costream::change_color(ConsoleColor color)
		{
			_current_color = color;
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
			Cprintf(">> ERROR", COLOR_PURPLE);
			std::cout << ": ";
			Cprintf(reason, COLOR_RED);
			std::cout << std::endl << std::endl;
		}

		//show message
		void ShowMessage(std::string message)
		{
			std::cout << ">> ";
			Cprintf(message, COLOR_WHITE);
			std::cout << std::endl << std::endl;
		}

		//warning check.
		void WarningCheck(bool condition, std::string reason, std::string file, int line, std::string function)
		{
			if (condition)
			{
				std::cout << std::endl << std::endl;
				Cprintf(">> WARNING: ", COLOR_PURPLE);
				Cprintf(reason, COLOR_RED);
				std::cout << std::endl;
				Cprintf("[File]: " + file, COLOR_GRAY);
				std::cout << std::endl;
				Cprintf("[Line]: " + ToString(line), COLOR_GRAY);
				std::cout << std::endl;
				Cprintf("[Func]: " + function, COLOR_GRAY);
				std::cout << std::endl;
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
			if (!system("clear"))
				std::cout << "clear failed" <<std::endl;
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
		//return true if the folder exists.
		bool DirExist(std::string dir_path)
		{
#ifdef __GADT_GNUC
			return (access(dir_path.c_str(), 0) != -1);
#elif defined(__GADT_MSVC)
			return (_access(dir_path.c_str(), 0) != -1);
#endif
		}

		//create dir and return true if create successfully.
		bool MakeDir(std::string dir_path)
		{
			if (DirExist(dir_path))
			{
				return true;
			}
#ifdef __GADT_GNUC
			return mkdir(dir_path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1;
			//return false;
#elif defined(__GADT_MSVC)
			return _mkdir(dir_path.c_str()) != -1;
#endif
		}

		//remove dir and return true if remove successfully. 
		bool RemoveDir(std::string dir_path)
		{
			if (!DirExist(dir_path))
			{
				return false;
			}
#ifdef __GADT_MSVC
			if (_rmdir(dir_path.c_str()) == 0)
#else
			if (rmdir(dir_path.c_str()) == 0)
#endif
				return true;
			std::cout << "[ERROR]: RemoveDir failed, ";
#ifdef __GADT_MSVC
			perror("rmdir");
#else
			perror("rmdir");
#endif
			return true;
		}

		//return true if the file exists.
		bool FileExist(std::string file_path)
		{
			std::ifstream ifs(file_path, std::ios::in);
			return ifs.operator bool();
		}

		//remove file and return true if remove successfully.
		bool RemoveFile(std::string file_path)
		{
			if (remove(file_path.c_str()) == 0)
				return true;
			std::cout << "[ERROR]: RemoveFile failed, ";
			perror("remove");
			return false;
		}

		//convert a file to string. return "" if convert failed.
		std::string FileToString(std::string file_path)
		{
			std::filebuf *pbuf;
			std::ifstream filestr;
			long size;
			char * buffer;
			filestr.open(file_path, std::ios::binary);
			pbuf = filestr.rdbuf();
			size = (long)pbuf->pubseekoff(0, std::ios::end, std::ios::in);
			pbuf->pubseekpos(0, std::ios::in);
			buffer = new char[size];
			pbuf->sgetn(buffer, size);
			filestr.close();
			//cout.write(buffer, size);
			std::string temp(buffer, size);
			delete[] buffer;
			return temp;
		}

		//convert a string to file.
		bool StringToFile(std::string str, std::string file_path)
		{
			std::ofstream ofs(file_path, std::ios::trunc);
			if (!ofs)
				return false;
			ofs << str;
			return true;
		}

	}

	namespace func
	{
		size_t GetManhattanDistance(Coordinate fir, Coordinate sec)
		{
			return abs(fir.x - sec.x) + abs(fir.y - sec.y);
		}
	}
}


