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
#include "gadt_log.hpp"
#include "gadt_container.hpp"

#pragma once

namespace gadt
{
	namespace console
	{
		//align mode of cell
		enum AlignMode : int8_t
		{
			TABLE_ALIGN_LEFT = 0,
			TABLE_ALIGN_MIDDLE = 1,
			TABLE_ALIGN_RIGHT = 2
		};

		//frame mode of table
		enum FrameMode : int8_t
		{
			TABLE_FRAME_DISABLE = 0,
			TABLE_FRAME_EMPTY = 1,
			TABLE_FRAME_HALF_EMPTY = 2,
			TABLE_FRAME_ENABLE = 3
		};

		//index type of table
		enum IndexMode : int8_t
		{
			TABLE_INDEX_DISABLE = -1,
			TABLE_INDEX_FROM_ZERO = 0,
			TABLE_INDEX_FROM_ONE = 1
		};

		//basic cell of table.
		struct TableCell
		{
			std::string				str;
			console::ConsoleColor	color;
			AlignMode				align;

			TableCell() :
				str(),
				color(console::COLOR_DEFAULT),
				align(TABLE_ALIGN_LEFT)
			{
			}

			TableCell(std::string _str) :
				str(_str),
				color(console::COLOR_DEFAULT),
				align(TABLE_ALIGN_LEFT)
			{
			}

			TableCell(std::string _str, console::ConsoleColor _color) :
				str(_str),
				color(_color),
				align(TABLE_ALIGN_LEFT)
			{
			}

			TableCell(std::string _str, AlignMode _align) :
				str(_str),
				color(console::COLOR_DEFAULT),
				align(_align)
			{
			}

			TableCell(std::string _str, console::ConsoleColor _color, AlignMode _align) :
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
					if (align == TABLE_ALIGN_LEFT)
					{
						temp += str;
						temp += std::string(space_width, ' ');
					}
					else if (align == TABLE_ALIGN_RIGHT)
					{
						temp += std::string(space_width, ' ');
						temp += str;
					}
					else if (align == TABLE_ALIGN_MIDDLE)
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
				align = TABLE_ALIGN_LEFT;
			}

			inline void set_middle_aligh()
			{
				align = TABLE_ALIGN_MIDDLE;
			}

			inline void set_right_aligh()
			{
				align = TABLE_ALIGN_RIGHT;
			}
		};

		//default width of each column of console table.
		const size_t DEFAULT_TABLE_WIDTH = 2;

		//console table
		class Table
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
			void BasicOutput(std::ostream & os, CellOutputFunc CellCallback, FrameMode frame_mode, IndexMode index_mode)
			{
				char frame[3];
				bool enable_index = (index_mode != TABLE_INDEX_DISABLE);
				switch (frame_mode)
				{
				case gadt::console::TABLE_FRAME_DISABLE:
					frame[0] = 0; frame[1] = 0; frame[2] = 0;
					break;
				case gadt::console::TABLE_FRAME_ENABLE:
					frame[0] = '+'; frame[1] = '-'; frame[2] = '|';
					break;
				default:
					frame[0] = ' '; frame[1] = ' '; frame[2] = ' ';
					break;
				}
				//std::string frame = enable_frame ? "+-|" : "   ";
				const size_t space_before_line_size = 4;
				std::string space_before_line(space_before_line_size, ' ');

				os << std::endl;

				//print indexs upper the table.

				if (enable_index)
				{
					os << space_before_line;
					size_t extra = frame_mode == TABLE_FRAME_DISABLE ? 0 : 1;
					for (size_t column = 0; column < this->number_of_columns(); column++)
					{
						std::string index = ToString(column + index_mode);
						os << index << std::string((_column_width[column] * 2) - index.length() + extra, ' ');
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
				if (frame_mode != TABLE_FRAME_DISABLE)
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
						std::string index = ToString(row + index_mode);
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

						if (frame_mode != TABLE_FRAME_DISABLE) { os << frame[2]; }
						if (column == this->number_of_columns() - 1) { os << std::endl; }
					}

					//print second line
					if (frame_mode != TABLE_FRAME_DISABLE && frame_mode != TABLE_FRAME_HALF_EMPTY)
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

			//get cell by coordinate
			inline const_reference get_cell(size_t column, size_t row) const
			{
				return _cells.element(column, row);
			}

			//get cell by coordinate
			inline const_reference get_cell(UnsignedCoordinate coord) const
			{
				return _cells.element(coord);
			}

			//get row by index
			inline CellPtrSet get_row(size_t index) const
			{
				return _cells.get_row(index);
			}

			//get column by index
			inline CellPtrSet get_column(size_t index) const
			{
				return _cells.get_column(index);
			}

			//enable title
			inline void enable_title(TableCell cell)
			{
				_enable_title = true;
				_title_cell = cell;
			}

			//disable title
			inline void disable_title()
			{
				_enable_title = false;
			}

			//set width for all columns
			inline void set_width(size_t width)
			{
				for (auto& value : _column_width)
					value = width;
			}

			//set width for appointed column
			inline void set_width(size_t column, size_t width)
			{
				GADT_CHECK_WARNING(GADT_TABLE_ENABLE_WARNING, column >= number_of_columns(), "TABLE02: out of column range.");
				_column_width[column] = width;
			}

			//set width by initializer list
			inline void set_width(std::initializer_list<size_t> width_list)
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

			//set cell by coordinate
			inline void set_cell(const_reference cell, size_t column, size_t row)
			{
				_cells.set_element(cell, column, row);
			}

			//set cell by coordinate
			inline void set_cell(const_reference cell, UnsignedCoordinate coord)
			{
				_cells.set_element(cell, coord);
			}

			//set cell in appointed row
			inline void set_cell_in_row(size_t row, TableCell cell)
			{
				_cells.set_row(row, cell);
			}

			//set cell in appointed row by initializer list
			inline void set_cell_in_row(size_t row, std::initializer_list<TableCell> cell_list)
			{
				_cells.set_row(row, cell_list);
			}

			//set cell in appointed column
			inline void set_cell_in_column(size_t column, TableCell cell)
			{
				_cells.set_column(column, cell);
			}

			//set cell in appointed column by initializer list
			inline void set_cell_in_column(size_t column, std::initializer_list<TableCell> cell_list)
			{
				_cells.set_column(column, cell_list);
			}

			//get string format of table.
			std::string output_string(FrameMode frame_mode = TABLE_FRAME_ENABLE, IndexMode index_mode = TABLE_INDEX_DISABLE)
			{
				CellOutputFunc cell_cb = [](const TableCell& c, size_t max_width, std::ostream& os)->void {
					os << c.to_string(max_width);
				};
				std::stringstream ss;
				BasicOutput(ss, cell_cb, frame_mode, index_mode);
				return ss.str();
			}

			//get cell reference by coordinate
			reference operator[](UnsignedCoordinate coord)
			{
				return _cells[coord];
			}

		public:

			//default constructor.
			Table() :
				_cells(0, 0),
				_column_width(0, DEFAULT_TABLE_WIDTH),
				_enable_title(false)
			{
			}

			//constructor function
			Table(size_t column_size, size_t row_size) :
				_cells(column_size, row_size),
				_column_width(column_size, DEFAULT_TABLE_WIDTH),
				_enable_title(false)
			{
			}

			//constructor function with initializer list(cell).
			Table(size_t column_size, size_t row_size, std::initializer_list<InitList> list) :
				_cells(column_size, row_size, list),
				_column_width(column_size, DEFAULT_TABLE_WIDTH),
				_enable_title(false)
			{
			}

			//constructor function with initializer list(string).
			Table(size_t column_size, size_t row_size, std::initializer_list<std::initializer_list<std::string>> list) :
				_cells(column_size, row_size, list),
				_column_width(column_size, DEFAULT_TABLE_WIDTH),
				_enable_title(false)
			{
			}

			//print table.
			void Print(FrameMode frame_mode = TABLE_FRAME_ENABLE, IndexMode index_mode = TABLE_INDEX_DISABLE)
			{
				CellOutputFunc callback = [](const TableCell& c, size_t max_width, std::ostream& os)->void {
					console::Cprintf(c.to_string(max_width), c.color);
				};
				BasicOutput(std::cout, callback, frame_mode, index_mode);
			}

			//increase row
			void IncreaseRow(size_t row_number)
			{
				_cells.IncreaseRow(row_number);
			}

			//decrease row
			void DecreaseRow(size_t row_number)
			{
				_cells.DecreaseRow(row_number);
			}

			//increase column
			void IncreaseColumn(size_t column_number)
			{
				_cells.IncreaseColumn(column_number);
			}

			//decrease column
			void DecreaseColumn(size_t column_number)
			{
				_cells.DecreaseColumn(column_number);
			}

			//resize the cells.
			void Resize(size_t column_size, size_t row_size)
			{
				_cells.Resize(column_size, row_size);
			}

			//load from stl::RectangeArray
			template<typename T, size_t WIDTH, size_t HEIGHT>
			void LoadRectangularArray(const stl::RectangularArray<T, WIDTH, HEIGHT>& rec_array, std::function<std::string(const T&)> trans_func)
			{
				Resize(0, 0);
				Resize(WIDTH, HEIGHT);
				for (auto coord : rec_array)
					_cells[coord] = TableCell(trans_func(rec_array[coord]));
			}

			//load from stl::RectangeArray
			template<typename T, size_t WIDTH, size_t HEIGHT>
			void LoadRectangularArray(const stl::RectangularArray<T, WIDTH, HEIGHT>& rec_array, std::function<TableCell(const T&)> trans_func)
			{
				Resize(0, 0);
				Resize(WIDTH, HEIGHT);
				for (auto coord : rec_array)
					_cells[coord] = trans_func(rec_array[coord]);
			}

			//load from stl::RectangeArray
			template<typename T>
			void LoadElementMartix(const stl::ElementMatrix<T>& martix, std::function<std::string(const T&)> trans_func)
			{
				Resize(0, 0);
				Resize(martix.width(), martix.height());
				for (auto coord : martix)
					_cells[coord] = TableCell(trans_func(martix[coord]));
			}

			//load from stl::RectangeArray
			template<typename T>
			void LoadElementMartix(const stl::ElementMatrix<T>& martix, std::function<TableCell(const T&)> trans_func)
			{
				Resize(0, 0);
				Resize(martix.width(), martix.height());
				for (auto coord : martix)
					_cells[coord] = trans_func(martix[coord]);
			}
		};
	}
}
