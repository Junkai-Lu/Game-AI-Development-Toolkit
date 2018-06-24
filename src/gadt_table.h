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
		enum class TableAlign : int8_t
		{
			Left = 0,
			Middle = 1,
			Right = 2
		};

		//frame mode of table
		enum class TableFrame : int8_t
		{
			Disable = 0,
			Basic = 1,
			Circle = 2,
			Enable = 3,
			DisableAndTight = 10,
			CircleAndTight = 12,
		};

		//index type of table
		enum class TableIndex : int8_t
		{
			Disable = -1,
			BeginFromZero = 0,
			BeginFromOne = 1
		};

		//basic cell of table.
		struct TableCell
		{
			std::string				str;
			console::ConsoleColor	color;
			TableAlign				align;

			TableCell();

			TableCell(std::string _str);

			TableCell(std::string _str, console::ConsoleColor _color);

			TableCell(std::string _str, TableAlign _align);

			TableCell(std::string _str, console::ConsoleColor _color, TableAlign _align);


			/*inline TableCell::TableCell() :
				str(),
				color(console::ConsoleColor::Default),
				align(TableAlign::Left)
			{
			}*/

			template<typename T, typename std::enable_if<!std::is_same<T, console::ConsoleColor>::value && !std::is_same<T, TableAlign>::value,int>::type = 0>
			TableCell(T value) :
				str(ToString<T>(value)),
				color(console::ConsoleColor::Default),
				align(TableAlign::Left)
			{
			}

			template<typename T, typename std::enable_if<!std::is_same<T, console::ConsoleColor>::value && !std::is_same<T, TableAlign>::value, int>::type = 0>
			TableCell(T value, console::ConsoleColor _color) :
				str(ToString<T>(value)),
				color(_color),
				align(TableAlign::Left)
			{
			}

			template<typename T, typename std::enable_if<!std::is_same<T, console::ConsoleColor>::value && !std::is_same<T, TableAlign>::value, int>::type = 0>
			TableCell(T value, TableAlign _align) :
				str(ToString<T>(value)),
				color(console::ConsoleColor::Default),
				align(_align)
			{
			}

			template<typename T, typename std::enable_if<!std::is_same<T, console::ConsoleColor>::value && !std::is_same<T, TableAlign>::value, int>::type = 0>
			TableCell(T value, console::ConsoleColor _color, TableAlign _align) :
				str(ToString<T>(value)),
				color(_color),
				align(_align)
			{
			}

			//set color of the cell.
			inline void set_color(ConsoleColor _color)
			{
				color = _color;
			}

			//set string of the cell.
			inline void set_string(std::string _str)
			{
				str = _str;
			}

			//set the cell to left align
			inline void set_left_aligh()
			{
				align = TableAlign::Left;
			}

			//set the cell to middile align.
			inline void set_middle_aligh()
			{
				align = TableAlign::Middle;
			}

			//set the cell to rignt align.
			inline void set_right_aligh()
			{
				align = TableAlign::Right;
			}

			//get string.
			std::string to_string(size_t max_length) const;
		};

		//default width of each column of console table.
		const size_t TABLE_DEFAULT_WIDTH = 2;

		//console table
		class Table
		{
		private:

			//define function type
			using pointer = TableCell*;
			using reference = TableCell&;
			using const_reference = const TableCell&;
			using InitList = std::initializer_list<TableCell>;
			using CellPtrSet = std::vector<pointer>;
			using CellOutputFunc = std::function<void(const TableCell&, size_t, std::ostream&)>;
			using FrameOutputFunc = std::function<void(std::string str, std::ostream&)>;

			//size
			stl::DynamicMatrix<TableCell> _cells;
			std::vector<size_t> _column_width;

			//title
			bool _enable_title;
			TableCell _title_cell;

			//color
			ConsoleColor _table_color;

		private:

			inline void print_index(size_t index, size_t max_width)
			{
				std::string index_str = gadt::ToString(index);
				Cprintf(index_str + std::string(max_width - index_str.length() + 1, ' '), _table_color);
			}

			//print cell
			inline void print_frame(std::string str)
			{
				Cprintf(str, _table_color);
			}

			//print cell
			inline void print_cell(const TableCell& c, size_t max_width)
			{
				console::Cprintf(c.to_string(max_width), c.color);
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

			//get cell by point
			inline const_reference get_cell(size_t column, size_t row) const
			{
				return _cells.element(column, row);
			}

			//get cell by point
			inline const_reference get_cell(UPoint point) const
			{
				return _cells.element(point);
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
				GADT_WARNING_IF(GADT_TABLE_ENABLE_WARNING, column >= number_of_columns(), "TABLE02: out of column range.");
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

			//set cell by point
			inline void set_cell(const_reference cell, size_t column, size_t row)
			{
				_cells.set_element(cell, column, row);
			}

			//set cell by point
			inline void set_cell(const_reference cell, UPoint point)
			{
				_cells.set_element(cell, point);
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

			//set table color.
			inline void set_table_color(ConsoleColor color)
			{
				_table_color = color;
			}

			//get cell reference by point
			reference operator[](UPoint point)
			{
				return _cells[point];
			}

		public:

			//default constructor.
			Table();

			//constructor function
			Table(size_t column_size, size_t row_size);

			//constructor function with initializer list(cell).
			Table(size_t column_size, size_t row_size, std::initializer_list<InitList> list);

			////constructor function with initializer list(string).
			//Table(size_t column_size, size_t row_size, std::initializer_list<std::initializer_list<std::string>> list);

			//print table.
			void Print(TableFrame frame_mode = TableFrame::Enable, TableIndex index_mode = TableIndex::Disable);

			//increase row
			void IncreaseRow(size_t row_number);

			//decrease row
			void DecreaseRow(size_t row_number);

			//increase column
			void IncreaseColumn(size_t column_number);

			//decrease column
			void DecreaseColumn(size_t column_number);

			//resize the cells.
			void Resize(size_t column_size, size_t row_size);

			//load from stl::StaticMatrix
			template<typename T, size_t WIDTH, size_t HEIGHT>
			void LoadFromStaticMatrix(const stl::StaticMatrix<T, WIDTH, HEIGHT>& matrix, std::function<TableCell(const T&)> ElemToCell)
			{
				stl::DynamicMatrix<TableCell> new_cells(WIDTH, HEIGHT);
				for (auto point : matrix)
					new_cells.set_element(ElemToCell(matrix.element(point)), point);
				std::vector<size_t> new_column_width(WIDTH, TABLE_DEFAULT_WIDTH);
				_cells = new_cells;
				_column_width = new_column_width;
			}

			//load from stl::StaticMatrix
			template<typename T, size_t WIDTH, size_t HEIGHT>
			void LoadFromStaticMatrix(const stl::StaticMatrix<T, WIDTH, HEIGHT>& matrix, std::function<std::string(const T&)> ElemToString)
			{
				std::function<TableCell(const T&)> ElemToCell = [&](const T& elem)->TableCell {
					return TableCell(ElemToString(elem));
				};
				LoadFromStaticMatrix<T, WIDTH, HEIGHT>(matrix, ElemToCell);
			}

			//load from stl::DynamicMatrix
			template<typename T>
			void LoadFromDynamicMatrix(const stl::DynamicMatrix<T>& matrix, std::function<TableCell(const T&)> ElemToCell)
			{
				stl::DynamicMatrix<TableCell> new_cells(matrix.width(), matrix.height());
				for (auto point : matrix)
					new_cells.set_element(ElemToCell(matrix.element(point)), point);
				std::vector<size_t> new_column_width(matrix.width(), TABLE_DEFAULT_WIDTH);
				_cells = new_cells;
				_column_width = new_column_width;
			}

			//load from stl::DynamicMatrix
			template<typename T>
			void LoadFromDynamicMatrix(const stl::DynamicMatrix<T>& matrix, std::function<std::string(const T&)> ElemToString)
			{
				std::function<TableCell(const T&)> ElemToCell = [&](const T& elem)->TableCell {
					return TableCell(ElemToString(elem));
				};
				LoadFromDynamicMatrix<T>(matrix, ElemToCell);
			}
		};
	}
}
