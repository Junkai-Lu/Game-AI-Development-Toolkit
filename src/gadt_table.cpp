/* Copyright (c) 2018 Junkai Lu <junkai-lu@outlook.com>.
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

namespace gadt
{
	namespace console
	{
		TableCell::TableCell() :
			str(),
			color(console::ConsoleColor::Default),
			align(TableAlign::Left)
		{
		}

		TableCell::TableCell(std::string _str) :
			str(_str),
			color(console::ConsoleColor::Default),
			align(TableAlign::Left)
		{
		}

		TableCell::TableCell(std::string _str, console::ConsoleColor _color) :
			str(_str),
			color(_color),
			align(TableAlign::Left)
		{
		}

		TableCell::TableCell(std::string _str, TableAlign _align) :
			str(_str),
			color(console::ConsoleColor::Default),
			align(_align)
		{
		}

		TableCell::TableCell(std::string _str, console::ConsoleColor _color, TableAlign _align) :
			str(_str),
			color(_color),
			align(_align)
		{
		}

		//get string from table cell.
		std::string TableCell::to_string(size_t max_length) const

		{
			std::string temp;
			if (str.length() < max_length)
			{
				size_t space_width = max_length - str.length();
				if (align == TableAlign::Left)
				{
					temp += str;
					temp += std::string(space_width, ' ');
				}
				else if (align == TableAlign::Right)
				{
					temp += std::string(space_width, ' ');
					temp += str;
				}
				else if (align == TableAlign::Middle)
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

		//default constructor.
		Table::Table() :
			_cells(0, 0),
			_column_width(0, TABLE_DEFAULT_WIDTH),
			_enable_title(false),
			_title_cell(),
			_table_color(ConsoleColor::Gray)
		{
		}

		//constructor function
		Table::Table(size_t column_size, size_t row_size) :
			_cells(column_size, row_size),
			_column_width(column_size, TABLE_DEFAULT_WIDTH),
			_enable_title(false),
			_title_cell(),
			_table_color(ConsoleColor::Gray)
		{
		}

		//constructor function with initializer list(cell).
		Table::Table(size_t column_size, size_t row_size, std::initializer_list<InitList> list) :
			_cells(column_size, row_size, list),
			_column_width(column_size, TABLE_DEFAULT_WIDTH),
			_enable_title(false),
			_title_cell(),
			_table_color(ConsoleColor::Gray)
		{
		}

		//print table
		void Table::Print(TableFrame frame_mode, TableIndex index_mode)
		{
			/*
			* Frame String Index
			* 0: outer horizon line
			* 1: outer vectical line
			* 2: center dot
			* 3: left dot
			* 4: right dot
			* 5: top dot
			* 6: bottom dot
			* 7: left top corner
			* 8: right top corner
			* 9: left bottom corner
			*10: right bottom corner
			*11: inner horizon line
			*12: inner vectical line
			*/

			if (number_of_columns() == 0 || number_of_rows() == 0)
				return;

			std::vector<std::string> FRAME;
			
			if (frame_mode == TableFrame::Disable || frame_mode == TableFrame::DisableAndTight)
			{
				FRAME = std::vector<std::string>(13, " ");
			}
			else if (frame_mode == TableFrame::Basic)
			{
				FRAME = std::vector<std::string>{
					"-",	//0: outer horizon line
					"|",	//1: outer vectical line
					"+",	//2: center dot
					"+",	//3: left dot
					"+",	//4: right dot
					"+",	//5: top dot
					"+",	//6: bottom dot
					"+",	//7: left top corner
					"+",	//8: right top corner
					"+",	//9: left bottom corner
					"+",	//10: right bottom corner
					"-",	//11: inner horizon line
					"|",	//12: inner vectical line
				};
			}
			else if (frame_mode == TableFrame::Circle || frame_mode == TableFrame::CircleAndTight)
			{
				FRAME = std::vector<std::string>{
					"─",	//0: outer horizon line
					"│",	//1: outer vectical line
					" ",	//2: center dot
					"│",	//3: left dot
					"│",	//4: right dot
					"─",	//5: top dot
					"─",	//6: bottom dot
					"┌",	//7: left top corner
					"┐",	//8: right top corner
					"└",	//9: left bottom corner
					"┘",	//10: right bottom corner
					" ",	//11: inner horizon line
					" ",	//12: inner vectical line
				};
			}
			else
			{
				FRAME = std::vector<std::string>{
					"─",	//0: outer horizon line
					"│",	//1: outer vectical line
					"┼",	//2: center dot
					"├",	//3: left dot
					"┤",	//4: right dot
					"┬",	//5: top dot
					"┴",	//6: bottom dot
					"┌",	//7: left top corner
					"┐",	//8: right top corner
					"└",	//9: left bottom corner
					"┘",	//10: right bottom corner
					"─",	//11: inner horizon line
					"│",	//12: inner vectical line
				};
			}

			bool enable_index = (index_mode != TableIndex::Disable);
			const size_t space_before_line_size = 4;
			std::string space_before_line(space_before_line_size, ' ');
			PrintEndLine();
			//print indexs upper the table.

			if (enable_index)
			{
				print_frame(std::string(space_before_line_size + 1, ' '));
				//size_t extra = 1;//frame_mode == TableFrame::Disable ? 0 : 1;
				for (size_t column = 0; column < this->number_of_columns(); column++)
					print_index(column + static_cast<size_t>(index_mode), _column_width[column] * 2);
				PrintEndLine();
			}

			//print title
			if (_enable_title)
			{
				size_t str_width = 1;
				for (auto w : _column_width) { str_width += (w * 2 + 1); }
				str_width -= 2;
				print_frame(space_before_line + FRAME[7]);
				for (size_t i = 0; i < str_width; i++)
					print_frame(FRAME[0]);//outer hor line
				print_frame(FRAME[8]);
				PrintEndLine(); 
				print_frame(space_before_line + FRAME[1]);
				print_cell(_title_cell, str_width);
				print_frame(FRAME[1]);
				PrintEndLine();
			}

			//print upper line of the table.
			if (frame_mode != TableFrame::Disable)
			{
				if (_enable_title)
					print_frame(space_before_line + FRAME[3]);//left dot
				else
					print_frame(space_before_line + FRAME[7]);//left top corner
				
				for (size_t column = 0; column < this->number_of_columns(); column++)
				{
					if (_enable_title)
					{
						for (size_t i = 0; i < _column_width[column] * 2; i++)
							print_frame(FRAME[11]);//inner line
					}
					else
					{
						for (size_t i = 0; i < _column_width[column] * 2; i++)
							print_frame(FRAME[0]);//outer line
					}
					
					if (column != this->number_of_columns() - 1)
					{
						print_frame(FRAME[5]);//top dot
					}
					else
					{
						if(_enable_title)
							print_frame(FRAME[4]);//right dot
						else
							print_frame(FRAME[8]);//top right corner
						PrintEndLine();
					}
				}
			}
			else
			{
				PrintEndLine();
			}

			for (size_t row = 0; row < this->number_of_rows(); row++)
			{
				//print first line , include value and space.
				if (enable_index)
					print_index(row + static_cast<size_t>(index_mode), space_before_line_size - 1);
				else
					print_frame(space_before_line);
				print_frame(FRAME[1]);//outer vec line
				for (size_t column = 0; column < this->number_of_columns(); column++)
				{
					const size_t width = _column_width[column] * 2;
					const TableCell& c = get_cell(column, row);
					print_cell(c, width);
					if (column == this->number_of_columns() - 1)//last column
					{
						print_frame(FRAME[1]); //outer vec line
						PrintEndLine();
					}
					else
					{
						print_frame(FRAME[12]);//inner vec line.
					}
				}

				//print second line
				if (row == this->number_of_rows() - 1 || (frame_mode != TableFrame::DisableAndTight && frame_mode != TableFrame::CircleAndTight))
				{
					if (row == this->number_of_rows() - 1)//last row
						print_frame(space_before_line + FRAME[9]);//left bottom corner
					else
						print_frame(space_before_line + FRAME[3]);//left dot
					for (size_t column = 0; column < this->number_of_columns(); column++)
					{
						if (row == this->number_of_rows() - 1)//last row
						{
							for (size_t i = 0; i < _column_width[column] * 2; i++)
								print_frame(FRAME[0]);//outer hor line
							if (column == this->number_of_columns() - 1)
							{
								print_frame(FRAME[10]);//right bottom corner
								PrintEndLine();
							}
							else
							{
								print_frame(FRAME[6]);//bottom dot
							}
						}
						else
						{
							for (size_t i = 0; i < _column_width[column] * 2; i++)
								print_frame(FRAME[11]);//inner hor line
							if (column == this->number_of_columns() - 1)
							{
								print_frame(FRAME[4]);//right dot
								PrintEndLine();
							}
							else
							{
								print_frame(FRAME[2]);
							}
						}
					}
				}
			}
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
