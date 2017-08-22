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
* LIABILITY, WHETHER IN AN T OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

// exclude unsupported compilers and define some marco by compiler.
#ifdef _MSC_VER
	#define __GADT_MSVC
	#include <SDKDDKVer.h>
	#include <Windows.h>
	#include <io.h>
	#include <direct.h>
	#include <tchar.h>
#elif defined(__GNUC__)
	#define __GADT_GNUC
	#if (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) < 40900
		#error "unsupported GCC version"
	#endif
	#include <sys/stat.h>  
	#include <sys/types.h>
	#include <errno.h> 
	#include <unistd.h>
	#include <dirent.h>
#else
	#error "unsupported compiler"
#endif

//open debug-info option to include extra info , this would lead to little performance penalties.
#define GADT_DEBUG_INFO

//warning option allow the functions in GADT execute parameter check and report wrong parameters.
#define GADT_WARNING

//a marco use for parameters check.
#ifdef GADT_WARNING
	#define GADT_CHECK_WARNING(enable, condition, reason) gadt::console::WarningCheck(enable && condition, reason, __FILE__, __LINE__, __FUNCTION__);
#else
	#define GADT_CHECK_WARNING(enable, condition, reason)
#endif

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <set>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <memory>
#include <functional>
#include <type_traits>

#pragma once

namespace gadt
{
	namespace console
	{
		//console color type
		enum ConsoleColor
		{
			DEFAULT = 7,
			DEEP_BLUE = 1,
			DEEP_GREEN = 2,
			DEEP_CYAN = 3,
			BROWN = 4,
			PURPLE = 5,
			DEEP_YELLOW = 6,
			DEEP_WHITE = 7,
			GRAY = 8,
			BLUE = 9,
			GREEN = 10,
			CYAN = 11,
			RED = 12,
			PINK = 13,
			YELLOW = 14,
			WHITE = 15
		};

		//color ostream.
		class costream
		{
		private:
			static ConsoleColor _current_color;

			//change colors by setting using winapi in windows or output string in linux.
			static void change_color(ConsoleColor color);

		public:
			costream& operator<<(ConsoleColor color)
			{
				change_color(color);
				return *this;
			}

			template<typename datatype>
			inline costream& operator<<(datatype d)
			{
				std::cout << d;
				return *this;
			}

			template <typename datatype>
			static inline void print(datatype data, ConsoleColor color)
			{
				ConsoleColor temp_color = _current_color;
				change_color(color);
				std::cout << data;
				change_color(temp_color);
			}
		};
		
		//bool to string. that can be replaced by '<< boolalpha'
		inline std::string BoolToString(bool b)
		{
			if (b)
			{
				return "true";
			}
			return "false";
		}

		//interger to string.
		inline std::string IntergerToString(size_t i)
		{
			std::ostringstream os;
			if (os << i) return os.str();
			return "invalid conversion";
		}

		//double to string
		inline std::string DoubleToString(double d)
		{
			std::ostringstream os;
			if (os << d) return os.str();
			return "invalid conversion";
		}

		//colorful print.
		template<typename T>
		inline void Cprintf(T data, ConsoleColor color)
		{
			costream::print<T>(data, color);
		}

		//show error in terminal.
		void ShowError(std::string reason);

		//show message in terminal.
		void ShowMessage(std::string message);

		//if 'condition' is true that report detail.
		void WarningCheck(bool condition, std::string reason, std::string file, int line, std::string function);

		//system pause.
		void SystemPause();

		//system clear
		void SystemClear();
	}

	namespace timer
	{
		std::string TimeString(std::string format = "%Y.%m.%d-%H:%M:%S");
		clock_t GetClock();
		double GetTimeDifference(const clock_t& start);
		
		class TimePoint
		{
		private:
			clock_t _clock;
			time_t _time;
		public:

			inline TimePoint()
			{
				reset();
			}

			//get string of this time point.
			std::string get_string(std::string format = "%Y.%m.%d-%H:%M:%S") const;

			//set this time point to current time.
			inline void reset()
			{
				_clock = clock();
				_time = time(NULL);
			}

			//get the time since this time point was created.
			inline double time_since_created() const
			{
				return (double)(clock() - _clock) / CLOCKS_PER_SEC;
			}
		};
	}

	namespace player
	{
		//PlayerIndex allows to define a index start from any number and with any length.
		template<int begin_index, size_t length>
		class PlayerIndex
		{
		protected:
			int _index;

		public:

			PlayerIndex() :_index(0) {}
			PlayerIndex(int index) :_index(index) {}

			//get next index. 
			inline int get_next() const
			{
				constexpr int end = length != 0 ? begin_index + (int)length : begin_index + 1;
				return _index + 1 < end ? _index + 1 : begin_index;
			}

			//get prev index.
			inline int get_prev() const
			{
				constexpr int end = length != 0 ? begin_index + (int)length : begin_index + 1;
				return _index - 1 < begin_index ? end - 1 : _index - 1;
			}

			//get index after jump.
			inline int get_jump(int value) const
			{
				constexpr int end = (length != 0 ? begin_index + (int)length : begin_index + 1);
				value = value % (int)length;
				return _index + value >= end ? _index - ((int)length - value) : _index + value;
			}

			//set index
			inline void set(int index)
			{
				_index = index;
			}

			//jump index.
			inline void jump(int value)
			{
				_index = get_jump(value);
			}

			//change to next index.
			inline void to_next()
			{
				_index = get_next();
			}

			//change to prev index.
			inline void to_prev()
			{
				_index = get_prev();
			}

			//get current index.
			inline int current() const
			{
				return _index;
			}
		};

		//PlayerGroup is an ordered container of player data that derived from PlayerIndex.
		template<typename Tdata, int begin_index, size_t length>
		class PlayerGroup: public PlayerIndex<begin_index, length>
		{
		protected:
			Tdata _data[length];

		public:

			PlayerGroup(){}
			PlayerGroup(int index) :PlayerIndex<begin_index, length>(index) {}

			//get data of current player.
			inline Tdata& data()
			{
				return _data[this->_index - begin_index];
			}

			//get data of apointed index.
			inline Tdata& data(size_t index)
			{
				return _data[index - begin_index];
			}

			//operateor[]
			inline Tdata& operator[](size_t index)
			{
				return data(index);
			}
		};
	}

	namespace file
	{
		//return true if the folder exists.
		bool DirExist(std::string path);
		
		//create dir and return true if create successfully.
		bool MakeDir(std::string path);

		//remove dir and return true if remove successfully. 
		bool RemoveDir(std::string path);
	}

	namespace log
	{
		//error log class. 
		class ErrorLog
		{
		private:
			std::vector<std::string> _error_list;

		public:
			//default constructor.
			ErrorLog()
			{

			}

			//constructor with init list.
			ErrorLog(std::initializer_list<std::string> init_list)
			{
				for (auto err : init_list)
				{
					_error_list.push_back(err);
				}
			}

			//copy constructor is banned
			ErrorLog(const ErrorLog&) = delete;

			//add a new error.
			inline void add(std::string err)
			{
				_error_list.push_back(err);
			}

			//return true if no error exist.
			inline bool is_empty() const
			{
				return _error_list.size() == 0;
			}

			//output as json format.
			inline std::string output() const
			{
				std::stringstream ss;
				ss << "[";
				for (size_t i = 0; i < _error_list.size(); i++)
				{
					ss << "\"" << _error_list[i] << "\"";
					if (i != _error_list.size() - 1)
					{
						ss << "," << std::endl;
					}
				}
				ss << "]";
				return ss.str();
			}
		};
	}

	namespace table
	{
		//enable warning in table.
		constexpr const bool g_TABLE_ENABLE_WARNING = true;

		//basic cell of table.
		struct TableCell
		{
			std::string				str;
			console::ConsoleColor	color;

			TableCell() :
				str(),
				color(console::DEFAULT)
			{
			}

			TableCell(std::string _str) :
				str(_str),
				color(console::DEFAULT)
			{
			}

			TableCell(console::ConsoleColor _color) :
				str(),
				color(_color)
			{
			}

			TableCell(std::string _str,console::ConsoleColor _color) :
				str(_str),
				color(_color)
			{
			}
		};

		//console table
		class ConsoleTable
		{
		private:
			using pointer = TableCell*;
			using reference = TableCell&;
			using CellSet = std::vector<std::vector<TableCell>>;
			using Column = std::vector<TableCell*>;
			using Row = std::vector<TableCell*>;	
			using CellOutputFunc = std::function<void(const TableCell&, std::ostream&, size_t)>;
			using FrameOutputFunc = std::function<void(std::string str, std::ostream&)>;

			const size_t _column_size;
			const size_t _row_size;

			CellSet				_cells;
			std::vector<Column> _column;
			std::vector<Row>	_row;
			std::vector<size_t> _column_width;
			
			static constexpr const size_t _default_width = 1;

		private:
			//initialize cells and column/row
			void init_cells();

			//basic output.
			void basic_output(std::ostream& os, CellOutputFunc cell_cb, FrameOutputFunc frame_cb, bool enable_frame, bool enable_index);

		public:
			//constructor function
			ConsoleTable(size_t column_size, size_t row_size);

			//constructor function with initializer list.
			ConsoleTable(size_t column_size, size_t row_size, std::initializer_list<std::initializer_list<std::string>> list);

			inline const Row& get_row(size_t index) 
			{
				GADT_CHECK_WARNING(g_TABLE_ENABLE_WARNING, index >= _row_size, "TABLE01: out of row range.");
				return _row[index]; 
			}

			inline const Column& get_column(size_t index) 
			{ 
				GADT_CHECK_WARNING(g_TABLE_ENABLE_WARNING, index >= _column_size, "TABLE02: out of column range.");
				return _column[index]; 
			}
			
			inline reference cell(size_t column, size_t row)
			{
				GADT_CHECK_WARNING(g_TABLE_ENABLE_WARNING, row >= _row_size, "TABLE01: out of row range.");
				GADT_CHECK_WARNING(g_TABLE_ENABLE_WARNING, column >= _column_size, "TABLE02: out of column range.");
				return (_cells[column])[row];
			}

			inline size_t row_size() const 
			{
				return _row_size; 
			}
			
			inline size_t column_size() const 
			{
				return _column_size; 
			}

			inline const Row& operator[](size_t index)
			{
				return get_row(index);
			}

			inline void set_width(size_t column, size_t width)
			{
				GADT_CHECK_WARNING(g_TABLE_ENABLE_WARNING, column >= _column_size, "TABLE02: out of column range.");
				_column_width[column] = width;
			}

			void set_width(std::initializer_list<size_t> width_list);

			void set_cell_in_row(size_t row, TableCell cell);

			void set_cell_in_row(size_t row, std::initializer_list<TableCell> cell_list);

			void set_cell_in_column(size_t column, TableCell cell);

			void set_cell_in_column(size_t column, std::initializer_list<TableCell> cell_list);

			std::string output_string(bool enable_frame = true, bool enable_index = true);

			void print(bool enable_frame = true, bool enable_index = true);
		};
	}
}