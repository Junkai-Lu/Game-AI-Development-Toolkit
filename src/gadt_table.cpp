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

#include "gadt_table.h"

#pragma once

namespace gadt
{
	namespace console
	{
		//get string from table cell.
		std::string TableCell::to_string(size_t max_length) const

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

		//basic output function.
		void Table::BasicOutput(std::ostream & os, CellOutputFunc CellCallback, FrameMode frame_mode, IndexMode index_mode)

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
					std::string index = gadt::ToString(column + index_mode);
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
					std::string index = gadt::ToString(row + index_mode);
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

		//convert the table to string.
		std::string Table::ConvertToString(FrameMode frame_mode, IndexMode index_mode)
		{
			CellOutputFunc cell_cb = [](const TableCell& c, size_t max_width, std::ostream& os)->void {
				os << c.to_string(max_width);
			};
			std::stringstream ss;
			BasicOutput(ss, cell_cb, frame_mode, index_mode);
			return ss.str();
		}

		//print table.
		void Table::Print(FrameMode frame_mode, IndexMode index_mode)
		{
			CellOutputFunc callback = [](const TableCell& c, size_t max_width, std::ostream& os)->void {
				console::Cprintf(c.to_string(max_width), c.color);
			};
			BasicOutput(std::cout, callback, frame_mode, index_mode);
		}

		//increase row
		void Table::IncreaseRow(size_t row_number)
		{
			_cells.IncreaseRow(row_number);
		}

		//decrease row
		void Table::DecreaseRow(size_t row_number)
		{
			_cells.DecreaseRow(row_number);
		}

		//increase column
		void Table::IncreaseColumn(size_t column_number)
		{
			_cells.IncreaseColumn(column_number);
		}

		//decrease column
		void Table::DecreaseColumn(size_t column_number)
		{
			_cells.DecreaseColumn(column_number);
		}

		//resize the cells.
		void Table::Resize(size_t column_size, size_t row_size)
		{
			_cells.Resize(column_size, row_size);
		}


	}
}
