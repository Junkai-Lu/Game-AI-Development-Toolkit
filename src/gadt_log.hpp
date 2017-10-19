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
#include "gadt_container.hpp"

#pragma once

namespace gadt
{
	namespace log
	{
		//error log class. 
		class ErrorLog
		{
		private:
			std::vector<std::string> _error_list;

		public:
			//default constructor.
			ErrorLog():
				_error_list()
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

		/*
		* SearchLogger is an template of log controller that is used for search logs.
		*
		* [State] is the game state type.
		* [Action] is the game action type.
		* [Result] is the game result type and could be ignore if it is unnecessary.
		*/
		template<typename State, typename Action, typename Result = int>
		class SearchLogger
		{
		private:
			using VisualTree = visual_tree::VisualTree;

		public:
			using StateToStrFunc = std::function<std::string(const State& state)>;
			using ActionToStrFunc = std::function<std::string(const Action& action)>;
			using ResultToStrFunc = std::function<std::string(const Result& result)>;

		private:
			//convert functions
			bool			_initialized;
			StateToStrFunc	_state_to_str_func;
			ActionToStrFunc	_action_to_str_func;
			ResultToStrFunc	_result_to_str_func;

			//log control
			bool			_enable_log;
			std::ostream*	_log_ostream;

			//json output control
			bool			_enable_json_output;
			std::string		_json_output_folder;
			VisualTree		_visual_tree;

		public:
			//default constructor.
			SearchLogger() :
				_initialized(false),
				_state_to_str_func([](const State&)->std::string {return ""; }),
				_action_to_str_func([](const Action&)->std::string {return ""; }),
				_enable_log(false),
				_log_ostream(&std::cout),
				_enable_json_output(false),
				_json_output_folder("JsonOutput"),
				_visual_tree()
			{
			}

			//initialized constructor.
			SearchLogger(
				StateToStrFunc state_to_str_func,
				ActionToStrFunc action_to_str_func,
				ResultToStrFunc result_to_str_func = [](const Result&)->std::string {return ""; }
			) :
				_initialized(true),
				_state_to_str_func(state_to_str_func),
				_action_to_str_func(action_to_str_func),
				_result_to_str_func(result_to_str_func),
				_enable_log(false),
				_log_ostream(std::cout),
				_enable_json_output(false),
				_json_output_folder("JsonOutput"),
				_visual_tree()
			{
			}

			//return true if log enabled.
			inline bool log_enabled() const { return _enable_log; }

			//return true if json output enabled
			inline bool json_output_enabled() const { return _enable_json_output; };

			//get log output stream.
			inline std::ostream& log_ostream() const
			{
				return *_log_ostream;
			}

			//get json output path
			inline std::string json_output_folder() const
			{
				return _json_output_folder;
			}

			//get ref of visual tree.
			inline VisualTree& visual_tree()
			{
				return _visual_tree;
			}

			//get state to string function.
			inline StateToStrFunc state_to_str_func() const
			{
				return _state_to_str_func;
			}

			//get action to string function.
			inline ActionToStrFunc action_to_str_func() const
			{
				return _action_to_str_func;
			}

			//get result to string function.
			inline ResultToStrFunc result_to_str_func() const
			{
				return _result_to_str_func;
			}

			//initialize logger.
			void Init(
				StateToStrFunc state_to_str_func,
				ActionToStrFunc action_to_str_func,
				ResultToStrFunc result_to_str_func
			)
			{
				_initialized = true;
				_state_to_str_func = state_to_str_func;
				_action_to_str_func = action_to_str_func;
				_result_to_str_func = result_to_str_func;
			}

			//enable log.
			inline void EnableLog(std::ostream& os)
			{
				_enable_log = true;
				_log_ostream = &os;
			}

			//disable log.
			inline void DisableLog()
			{
				_enable_log = false;
			}

			//enable json output
			inline void EnableJsonOutput(std::string json_output_folder)
			{
				_enable_json_output = true;
				_json_output_folder = json_output_folder;
			}

			//disable json output.
			inline void DisableJsonOutput()
			{
				_enable_json_output = false;
			}

			//output json to path.
			inline void OutputJson() const
			{
				std::string path = "./" + _json_output_folder;
				if (!file::DirExist(path))
				{
					file::MakeDir(path);
				}
				path += "/";
				path += timer::TimeString();
				path += ".json";
				std::ofstream ofs(path);
				_visual_tree.output_json(ofs);
			}

		};

		//align type
		enum AlignType : int8_t
		{
			ALIGN_LEFT = 0,
			ALIGN_MIDDLE = 1,
			ALIGN_RIGHT = 2
		};

		//basic cell of table.
		struct TableCell
		{
			std::string				str;
			console::ConsoleColor	color;
			AlignType				align;

			TableCell() :
				str(),
				color(console::DEFAULT),
				align(ALIGN_LEFT)
			{
			}

			TableCell(std::string _str) :
				str(_str),
				color(console::DEFAULT),
				align(ALIGN_LEFT)
			{
			}

			TableCell(std::string _str, console::ConsoleColor _color) :
				str(_str),
				color(_color),
				align(ALIGN_LEFT)
			{
			}

			TableCell(std::string _str, AlignType _align) :
				str(_str),
				color(console::DEFAULT),
				align(_align)
			{
			}

			TableCell(std::string _str, console::ConsoleColor _color, AlignType _align) :
				str(_str),
				color(_color),
				align(_align)
			{
			}

			//get string.
			std::string to_string(size_t max_length) const
			{
				std::string temp;
				if (str.length() < max_length)
				{
					size_t space_width = max_length - str.length();
					if (align == ALIGN_LEFT)
					{
						temp += str;
						temp += std::string(space_width, ' ');
					}
					else if (align == ALIGN_RIGHT)
					{
						temp += std::string(space_width, ' ');
						temp += str;
					}
					else if (align == ALIGN_MIDDLE)
					{
						size_t left_width = space_width / 2;
						size_t right_width = space_width - left_width;
						temp += std::string(left_width, ' ');
						temp += str;
						temp += std::string(right_width, ' ');
					}
				}
				else
				{
					temp = str.substr(0, max_length);
				}
				return temp;
			}

			inline void set_left_aligh()
			{
				align = ALIGN_LEFT;
			}

			inline void set_middle_aligh()
			{
				align = ALIGN_MIDDLE;
			}

			inline void set_right_aligh()
			{
				align = ALIGN_RIGHT;
			}
		};

		//console table
		class ConsoleTable
		{
		private:
			using pointer = TableCell*;
			using reference = TableCell&;
			using const_reference = const TableCell&;
			using InitList = std::initializer_list<TableCell>;
			using CellPtrSet = std::vector<pointer>;
			using CellOutputFunc = std::function<void(const TableCell&, size_t, std::ostream&)>;
			using FrameOutputFunc = std::function<void(std::string str, std::ostream&)>;
			
			//size
			stl::ElementMatrix<TableCell> _cells;
			std::vector<size_t> _column_width;

			//title
			bool _enable_title;
			TableCell _title_cell;

		private:

			//basic output.
			void BasicOutput(std::ostream & os, CellOutputFunc CellCallback, bool enable_frame, bool enable_index)
			{
				std::string frame = enable_frame ? "+-|" : "   ";
				const size_t space_before_line_size = 4;
				std::string space_before_line(space_before_line_size, ' ');

				os << std::endl;

				//print indexs upper the table.

				if (enable_index)
				{
					os << space_before_line << " ";
					for (size_t column = 0; column < this->number_of_columns(); column++)
					{
						std::string index = ToString(column + 1);
						os << index << std::string((_column_width[column] * 2) - index.length() + 1, ' ');
					}
					os << std::endl;
				}

				//print title
				if (_enable_title)
				{
					size_t str_width = 1;
					for (auto w : _column_width) { str_width += (w * 2 + 1); }
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
					for (size_t column = 0; column < this->number_of_columns(); column++)
					{
						os << std::string(_column_width[column] * 2, frame[1]) << frame[0];
						if (column == this->number_of_columns() - 1)
						{
							os << std::endl;
						}
					}
				}

				for (size_t row = 0; row < this->number_of_rows(); row++)
				{
					//print first line , include value and space.
					if (enable_index)
					{
						std::string index = ToString(row + 1);
						os << ' ' << index << std::string(space_before_line_size - index.length() - 1, ' ');
					}
					else
					{
						os << space_before_line;
					}
					os << frame[2];
					for (size_t column = 0; column < this->number_of_columns(); column++)
					{
						const size_t width = _column_width[column] * 2;
						const TableCell& c = get_cell(column, row);
						CellCallback(c, width, os);
						if (column != this->number_of_columns() - 1) { os << frame[2]; }
						else
						{
							os << frame[2] << std::endl;
						}
					}

					//print second line
					if (enable_frame)
					{
						os << space_before_line << frame[0];
						for (size_t column = 0; column < this->number_of_columns(); column++)
						{
							os << std::string(_column_width[column] * 2, frame[1]);
							os << frame[0];
							if (column == this->number_of_columns() - 1)
							{
								os << std::endl;
							}
						}
					}
				}

			}

		public:
			//constructor function
			ConsoleTable(size_t column_size, size_t row_size) :
				_cells(column_size, row_size),
				_column_width(column_size, 2),
				_enable_title(false)
			{
			}

			//constructor function with initializer list(cell).
			ConsoleTable(size_t column_size, size_t row_size, std::initializer_list<InitList> list):
				_cells(column_size, row_size, list),
				_column_width(column_size, 2),
				_enable_title(false)
			{
			}

			//constructor function with initializer list(string).
			ConsoleTable(size_t column_size, size_t row_size, std::initializer_list<std::initializer_list<std::string>> list) :
				_cells(column_size, row_size, list),
				_column_width(column_size, 2),
				_enable_title(false)
			{
			}

			//get number of rows.
			inline size_t number_of_rows() const
			{
				return _cells.height();
			}

			//get number of columns.
			inline size_t number_of_columns() const
			{
				return _cells.width();
			}

			inline const_reference get_cell(size_t column, size_t row) const
			{
				return _cells.element(column, row);
			}

			inline const_reference get_cell(Coordinate coord) const
			{
				return _cells.element(coord);
			}

			inline CellPtrSet get_row(size_t index) const
			{
				return _cells.get_row(index);
			}

			inline CellPtrSet get_column(size_t index) const
			{
				return _cells.get_column(index);
			}

			inline void enable_title(TableCell cell)
			{
				_enable_title = true;
				_title_cell = cell;
			}

			inline void disable_title()
			{
				_enable_title = false;
			}

			void set_width(size_t column, size_t width)
			{
				GADT_CHECK_WARNING(GADT_TABLE_ENABLE_WARNING, column >= number_of_columns(), "TABLE02: out of column range.");
				_column_width[column] = width;
			}

			void set_width(std::initializer_list<size_t> width_list)
			{
				size_t i = 0;
				for (size_t width : width_list)
				{
					if (i < number_of_columns())
					{
						_column_width[i] = width;
					}
					i++;
				}
			}

			inline void set_cell(const_reference cell, size_t column, size_t row)
			{
				_cells.set_element(cell, column, row);
			}

			inline void set_cell(const_reference cell, Coordinate coord)
			{
				_cells.set_element(cell, coord);
			}

			inline void set_cell_in_row(size_t row, TableCell cell)
			{
				_cells.set_row(row, cell);
			}

			inline void set_cell_in_row(size_t row, std::initializer_list<TableCell> cell_list)
			{
				_cells.set_row(row, cell_list);
			}

			inline void set_cell_in_column(size_t column, TableCell cell)
			{
				_cells.set_column(column, cell);
			}

			inline void set_cell_in_column(size_t column, std::initializer_list<TableCell> cell_list)
			{
				_cells.set_column(column, cell_list);
			}

			std::string output_string(bool enable_frame = true, bool enable_index = false)
			{
				CellOutputFunc cell_cb = [](const TableCell& c, size_t max_width, std::ostream& os)->void {
					os << c.to_string(max_width);
				};
				std::stringstream ss;
				BasicOutput(ss, cell_cb, enable_frame, enable_index);
				return ss.str();
			}

			void print(bool enable_frame = true, bool enable_index = false)
			{
				CellOutputFunc callback = [](const TableCell& c, size_t max_width, std::ostream& os)->void {
					console::Cprintf(c.to_string(max_width), c.color);
				};
				BasicOutput(std::cout, callback, enable_frame, enable_index);
			}

			reference operator[](Coordinate coord)
			{
				return _cells[coord];
			}
		};
	}
}
