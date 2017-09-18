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
		//set global console color.
		ConsoleColor costream::_current_color = DEFAULT;

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
			Cprintf(">> ERROR", PURPLE);
			std::cout << ": ";
			Cprintf(reason, RED);
			std::cout << std::endl << std::endl;
		}

		//show message
		void ShowMessage(std::string message)
		{
			std::cout << ">> ";
			Cprintf(message, WHITE);
			std::cout << std::endl << std::endl;
		}

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

		void SystemPause()
		{
#ifdef __GADT_GNUC
			cout << "Press ENTER to continue." << endl;
			while (!getchar());
#elif defined(__GADT_MSVC)
			system("pause");
#endif
		}
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

	namespace table
	{
		const size_t ConsoleTable::_default_width = 1;

		void ConsoleTable::init_cells()
		{
			//init columns
			for (size_t column = 0; column < _column_size; column++)
			{
				Column temp;
				for (size_t row = 0; row < _row_size; row++)
				{
					temp.push_back(&((_cells[column])[row]));
				}
				_column.push_back(temp);
			}

			//init rows
			for (size_t row = 0; row < _row_size; row++)
			{
				Row temp;
				for (size_t column = 0; column < _column_size; column++)
				{
					temp.push_back(&((_cells[column])[row]));
				}
				_row.push_back(temp);
			}
		}

		void ConsoleTable::basic_output(std::ostream & os, CellOutputFunc CellCallback, bool enable_frame, bool enable_index)
		{
			std::string frame = enable_frame ? "+-|" : "   ";
			const size_t space_before_line_size = 4;
			std::string space_before_line(space_before_line_size, ' ');
			
			os << std::endl;

			//print indexs upper the table.
			
			if (enable_index )
			{
				os << space_before_line << " ";
				for (size_t column = 0; column < _column_size; column++)
				{
					std::string index = console::IntergerToString(column + 1);
					os << index << std::string((_column_width[column] * 2) - index.length() + 1, ' ');
				}
				os << std::endl;
			}

			//print title
			if (_enable_title)
			{
				size_t str_width = 1;
				for (auto w : _column_width) { str_width += (w *2 + 1); }
				str_width -= 2;
				os << space_before_line << frame[0];
				os << std::string(str_width, frame[1]);
				os << frame[0] << std::endl;

				os << space_before_line << frame[2];
				CellCallback(_title_cell, str_width, os);
				os << frame[2] << std::endl;
			}

			//print upper line of the table.
			if (enable_frame)
			{
				os << space_before_line << frame[0];
				for (size_t column = 0; column < _column_size; column++)
				{
					os << std::string(_column_width[column] * 2, frame[1]) << frame[0];
					if (column == _column_size - 1)
					{
						os << std::endl;
					}
				}
			}

			for (size_t row = 0; row < _row_size; row++)
			{
				//print first line , include value and space.
				if (enable_index)
				{
					std::string index = console::IntergerToString(row + 1);
					os << ' ' << index << std::string(space_before_line_size - index.length() - 1, ' ');
				}
				else
				{
					os << space_before_line;
				}
				os << frame[2];
				for (size_t column = 0; column < _column_size; column++)
				{
					const size_t width = _column_width[column] * 2;
					const TableCell& c = cell(column, row);
					CellCallback(c, width, os);
					if (column != _column_size - 1) { os << frame[2]; }
					else 
					{
						os << frame[2] << std::endl;
					}
				}

				//print second line
				if (enable_frame)
				{
					os << space_before_line << frame[0];
					for (size_t column = 0; column < _column_size; column++)
					{
						os << std::string(_column_width[column] * 2, frame[1]);
						os << frame[0];
						if (column == _column_size - 1)
						{
							os << std::endl;
						}
					}
				}
			}

		}

		ConsoleTable::ConsoleTable(size_t column_size, size_t row_size) :
			_column_size(column_size),
			_row_size(row_size),
			_cells(column_size, std::vector<TableCell>(row_size, TableCell())),
			_column_width(column_size,_default_width),
			_enable_title(false)
		{
			init_cells();
		}

		ConsoleTable::ConsoleTable(size_t column_size, size_t row_size, std::initializer_list<std::initializer_list<std::string>> list) :
			_column_size(column_size),
			_row_size(row_size),
			_cells(column_size, std::vector<TableCell>(row_size, TableCell())),
			_column_width(column_size, _default_width),
			_enable_title(false)
		{
			init_cells();
			size_t y = 0;
			for (auto row : list)
			{
				size_t x = 0;
				for (auto value : row)
				{
					if (x < _column_size && y < _row_size)
					{
						((_cells[x])[y]).str = value;
					}
					x++;
				}
				y++;
			}
		}
		
		void ConsoleTable::set_width(std::initializer_list<size_t> width_list)
		{
			size_t i = 0;
			for (size_t width : width_list)
			{
				if (i < column_size())
				{
					_column_width[i] = width;
				}
				i++;
			}
		}

		void ConsoleTable::set_cell_in_row(size_t row, TableCell cell)
		{
			for (pointer cell_ptr : get_row(row))
			{
				*cell_ptr = cell;
			}
		}

		void ConsoleTable::set_cell_in_row(size_t row, std::initializer_list<TableCell> cell_list)
		{
			size_t i = 0;
			for (const TableCell& cell : cell_list)
			{
				if (i < get_row(row).size())
				{
					*get_row(row)[i] = cell;
				}
				i++;
			}
		}

		void ConsoleTable::set_cell_in_column(size_t column, TableCell cell)
		{
			for (pointer cell_ptr : get_column(column))
			{
				*cell_ptr = cell;
			}
		}

		void ConsoleTable::set_cell_in_column(size_t column, std::initializer_list<TableCell> cell_list)
		{
			size_t i = 0;
			for (const TableCell& cell : cell_list)
			{
				if (i < get_column(column).size())
				{
					*get_column(column)[i] = cell;
				}
				i++;
			}
		}
		
		//output string
		std::string ConsoleTable::output_string(bool enable_frame, bool enable_index)
		{
			CellOutputFunc cell_cb = [](const TableCell& c, size_t max_width, std::ostream& os )->void {
				os << c.GetString(max_width);
			};
			std::stringstream ss;
			basic_output(ss, cell_cb, enable_frame, enable_index);
			return ss.str();
		}

		//print 
		void ConsoleTable::print(bool enable_frame, bool enable_index)
		{
			CellOutputFunc callback = [](const TableCell& c, size_t max_width, std::ostream& os)->void {
				console::Cprintf(c.GetString(max_width), c.color);
			};
			basic_output(std::cout, callback, enable_frame, enable_index);
		}
	}
}