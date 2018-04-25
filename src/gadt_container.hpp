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
#include "gadt_memory.hpp"

#pragma once

namespace gadt
{
	namespace stl
	{
		/*
		* ListNode is the basic unit of gadt::stl::List.
		*
		* [T] is the class type of the link list.
		* [is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename T>
		struct ListNode
		{
		public:
			using pointer = ListNode<T>*;

		private:
			const T _value;
			pointer _next_node;
			pointer _prev_node;

		public:
			//constructor function.
			inline ListNode(const T& value) :
				_value(value),
				_next_node(nullptr),
				_prev_node(nullptr)
			{
			}

			//copy constructor function is disallowed.
			ListNode(const ListNode&) = delete;

			inline const T& value() const { return _value; }
			inline pointer next_node() const { return _next_node; }
			inline pointer prev_node() const { return _prev_node; }
			inline void set_next_node(pointer p) { _next_node = p; }
			inline void set_prev_node(pointer p) { _prev_node = p; }
		};

		/*
		* List is a template of link list.
		*
		* [T] is the class type of the link list.
		* [is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename T, bool _is_debug = false>
		class List
		{
		public:
			using Node = ListNode<T>;
			using Allocator = gadt::stl::StackAllocator<Node, _is_debug>;
			using node_pointer = Node*;

		private:
			const bool   _private_allocator;
			Allocator&	 _allocator;
			node_pointer _first_node;
			node_pointer _last_node;
			node_pointer _iterator;
			size_t       _size;

		private:
			constexpr inline  bool is_debug() const
			{
				return _is_debug;
			}

		public:
			List(size_t allocator_count) :
				_private_allocator(true),
				_allocator(*(new Allocator(allocator_count))),
				_first_node(nullptr),
				_last_node(nullptr),
				_iterator(nullptr),
				_size(0)
			{
			}

			List(Allocator& allocator) :
				_private_allocator(false),
				_allocator(allocator),
				_first_node(nullptr),
				_last_node(nullptr),
				_iterator(nullptr),
				_size(0)
			{
			}

			inline ~List()
			{
				if (_private_allocator)
				{
					delete &_allocator;
				}
			}

			//insert a new value in the end of the list.
			bool push_back(const T& value)
			{
				_size++;//incr size of the list.
				auto ptr = _allocator.construct(value);
				if (ptr == nullptr)
					return false;//constructor failed.
				if (_first_node == nullptr)
				{
					_first_node = ptr;
					_last_node = ptr;
					_iterator = ptr;
				}
				else
				{
					_last_node->set_next_node(ptr);
					ptr->set_prev_node(_last_node);
					_last_node = _last_node->next_node();
				}
				return true;
			}

			bool push_front(const T& value)
			{
				_size++;
				auto ptr = _allocator.construct(value);
				if (ptr == nullptr)
					return false;//constructor failed.
				if (_first_node == nullptr)
				{
					_first_node = ptr;
					_last_node = ptr;
					_iterator = ptr;
				}
				else
				{
					ptr->set_next_node(_first_node);
					_first_node->set_prev_node(ptr);
					_first_node = ptr;
				}
				return true;
			}

			void pop_back()
			{
				GADT_CHECK_WARNING(is_debug(), _last_node == nullptr, "no element in the back of link list");
				node_pointer temp = _last_node;
				_last_node = _last_node->prev_node();
				_last_node->set_next_node(nullptr);
				_allocator.destory(temp);
			}

			void pop_front()
			{
				GADT_CHECK_WARNING(is_debug(), _first_node == nullptr, "no element in the front of link list");
				node_pointer temp = _first_node;
				_first_node = _first_node->next_node();
				_first_node->set_prev_node(nullptr);
				_allocator.destory(temp);
			}

			//get the size of the list.
			inline size_t size() const
			{
				return _size;
			}

			//clear all nodes from allocator.
			void clear()
			{
				node_pointer ptr = _first_node;
				if (_first_node != nullptr)//to avoid the first node is not exist.
				{
					for (;;)
					{
						node_pointer temp_ptr = ptr->next_node();
						_allocator.destory(ptr);
						if (temp_ptr == nullptr)
						{
							break;
						}
						ptr = temp_ptr;
					}
				}
				_first_node = nullptr;
				_last_node = nullptr;
				_iterator = nullptr;
			}

			//to next iterator.
			bool to_next_iterator()
			{
				if (_iterator != nullptr)
				{
					_iterator = _iterator->next_node();
					return true;
				}
				return false;
			}

			//get T from iterator.
			inline const T& iterator() const
			{
				return _iterator->value();
			}

			//reset iterator from begin.
			inline void reset_iterator()
			{
				_iterator = _first_node;
			}

			//return true if the iterator point to the first node.
			inline bool is_begin() const
			{
				return _iterator == _first_node;
			}

			//return true if the iterator point to the last node.
			inline bool is_end() const
			{
				return _iterator == nullptr;
			}

			//get first itertor.
			inline node_pointer begin() const { return _first_node; }

			//get last iterator.
			inline node_pointer end() const { return _last_node; }
		};

		/*
		* RandomPoolElement the basic part of RandomPool.
		*
		* [T] is the type of element.
		*/
		template<typename T>
		struct RandomPoolElement
		{
			//left is close and right is open. 
			const size_t weight;
			const size_t left;
			const size_t right;
			T data;

			template<class... Types>
			RandomPoolElement(size_t _weight, size_t _left, size_t _right, Types&&... args) :
				weight(_weight),
				left(_left),
				right(_right),
				data(std::forward<Types>(args)...)
			{
			}

			std::string range() const
			{
				std::stringstream ss;
				ss << "[ " << left << " , " << right << " )";
				return ss.str();
			}
		};

		/*
		* RectangularArrayIter is the iterator of matrix container.
		*/
		class MatrixIter
		{
		private:
			UnsignedCoordinate _coord;
			const size_t _width;
			const size_t _height;

		public:
			MatrixIter(UnsignedCoordinate coord, size_t width, size_t height) :
				_coord(coord),
				_width(width),
				_height(height)
			{
			}

			bool operator!=(const MatrixIter& iter) const
			{
				return _coord != iter._coord;
			}

			void operator++()
			{
				_coord.x++;
				if (_coord.x >= _width)
				{
					_coord.x = 0;
					_coord.y++;
				}
			}

			UnsignedCoordinate operator* ()
			{
				return _coord;
			}
		};

		/*template<typename T>
		class MatrixBase
		{
			//get number of rows.
			virtual size_t height() const = 0;

			//get number of columns.
			virtual size_t width() const = 0;

			//get element
			virtual const T& element(Coordinate coord) const = 0;

			//get element
			virtual const T& element(size_t x, size_t y) const = 0;
		};*/

		/*
		* ElementMatrix is a flexiable matrix.
		*
		* [T] is the type of element.
		*/
		template<typename T>
		class ElementMatrix
		{
		private:
			using pointer = T*;
			using reference = T&;
			using const_reference = const T&;
			using Element = T;
			using ElementSet = std::vector<std::vector<T>>;
			using Row = std::vector<pointer>;
			using Column = Row;
			using InitList = std::initializer_list<T>;
			using Iter = MatrixIter;

		private:

			size_t				_width;
			size_t				_height;
			ElementSet			_elements;
			std::vector<Column> _column;
			std::vector<Row>	_row;

		private:
			//initialize columns and rows.
			void init_column_and_row()
			{
				//init columns
				for (size_t column = 0; column < _width; column++)
				{
					Column temp;
					for (size_t row = 0; row < _height; row++)
					{
						temp.push_back(&((_elements[column])[row]));
					}
					_column.push_back(temp);
				}

				//init rows
				for (size_t row = 0; row < _height; row++)
				{
					Row temp;
					for (size_t column = 0; column < _width; column++)
					{
						temp.push_back(&((_elements[column])[row]));
					}
					_row.push_back(temp);
				}
			}

			//return true if the coordinate is legal
			inline bool is_legal_coordinate(size_t x, size_t y) const
			{
				return x < _width && y < _height;
			}

		public:

			//get number of rows.
			inline size_t height() const 
			{
				return _height;
			}

			//get number of columns.
			inline size_t width() const 
			{
				return _width;
			}

			//get element
			inline const_reference element(UnsignedCoordinate coord) const 
			{
				return element(coord.x, coord.y);
			}

			//get element
			inline const_reference element(size_t x, size_t y) const 
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, !is_legal_coordinate(x,y), "out of row range.");
				return (_elements[x])[y];
			}

			//get row by index.
			inline const Row& get_row(size_t index) const
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, index >= _height, "out of row range.");
				return _row[index];
			}

			//get column by index.
			inline const Column& get_column(size_t index) const
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, index >= _width, "out of column range.");
				return _column[index];
			}

			//set element.
			inline void set_element(const_reference elem)
			{
				for (std::vector<Element>& row : _elements)
					for (Element& e : row)
						e = elem;
			}

			//set element.
			inline void set_element(const_reference elem, size_t x, size_t y)
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, !is_legal_coordinate(x, y), "out of range.");
				(_elements[x])[y] = elem;
			}

			//set element.
			inline void set_element(const_reference elem, UnsignedCoordinate coord)
			{
				set_element(elem, coord.x, coord.y);
			}

			//set row as same element.
			inline void set_row(size_t row_index, const_reference elem)
			{
				for (pointer elem_ptr : get_row(row_index))
					*elem_ptr = elem;
			}

			//set row by init list.
			inline void set_row(size_t row_index, InitList list)
			{
				size_t i = 0;
				for (const_reference elem : list)
				{
					if (i < get_row(row_index).size())
						*get_row(row_index)[i] = elem;
					else
						break;
					i++;
				}
			}

			//set column as same element.
			inline void set_column(size_t column_index, const_reference elem)
			{
				for (pointer elem_ptr : get_column(column_index))
					*elem_ptr = elem;
			}

			//set column bt init list.
			inline void set_column(size_t column_index, InitList list)
			{
				size_t i = 0;
				for (const_reference elem : list)
				{
					if (i < get_column(column_index).size())
						*get_column(column_index)[i] = elem;
					else
						break;
					i++;
				}
			}

			//return true if element exist.
			inline bool any(const_reference elem) const
			{
				for (auto coord : *this)
				{
					if (element(coord) == elem)
						return true;
				}
				return false;
			}

			//return true if element do not exist.
			inline bool none(const_reference elem) const
			{
				for (auto coord : *this)
				{
					if (element(coord) == elem)
						return false;
				}
				return true;
			}

			//get iterator begin.
			inline Iter begin() const
			{
				return Iter({ 0, 0 }, width(), height());
			}

			//get iterator end.
			inline Iter end() const
			{
				return Iter({ 0, height() }, width(), height());
			}

			//get element by coordinate.
			inline reference operator[](UnsignedCoordinate coord)
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, !is_legal_coordinate(coord.x, coord.y), "out of row range.");
				return (_elements[coord.x])[coord.y];
			}

		public:

			//constructor function
			ElementMatrix(size_t width, size_t height, const_reference elem = Element()) :
				_width(width),
				_height(height),
				_elements(width, std::vector<T>(height, elem))
			{
				init_column_and_row();
			}

			//constructor function with initializer list.
			template<typename ParamType>
			ElementMatrix(size_t column_size, size_t row_size, std::initializer_list<std::initializer_list<ParamType>> list) :
				_width(column_size),
				_height(row_size),
				_elements(column_size, std::vector<T>(row_size, Element()))
			{
				init_column_and_row();
				size_t y = 0;
				for (auto row : list)
				{
					size_t x = 0;
					for (auto value : row)
					{
						if (x < _width && y < _height)
						{
							((_elements[x])[y]) = Element(value);
						}
						x++;
					}
					y++;
				}
			}

			//increase rows.
			void IncreaseRow(size_t row_num, Element elem = Element())
			{
				for (size_t i = 0; i < row_num; i++)
				{
					Row new_row;
					for (std::vector<Element>& column : _elements)
					{
						column.push_back(elem);//add new element in the end of each column.
						new_row.push_back(&column.back());
					}
					_row.push_back(new_row);
					_height++;
				}
			}

			//decrease rows.
			void DecreaseRow(size_t row_num)
			{
				if (_height > row_num)
					_height -= row_num;
				else
					_height = 0;
				_row.resize(_height);
				for (std::vector<Element>& column : _elements)
					column.resize(_height);
			}

			//increase column
			void IncreaseColumn(size_t column_num, Element elem = Element())
			{
				for (size_t i = 0; i < column_num; i++)
				{
					_elements.push_back(std::vector<Element>(_height, elem));
					Column new_column;
					for (size_t y = 0; y < _height; y++)
						new_column.push_back(&_elements.back()[y]);
					_column.push_back(new_column);
					_width++;
				}
			}

			//decrease column
			void DecreaseColumn(size_t column_num)
			{
				if (_width > column_num)
					_width -= column_num;
				else
					_width = 0;
				_column.resize(_width);
				_elements.resize(_width);
			}

			//resize the martix.
			void Resize(size_t new_width, size_t new_height)
			{
				if (new_height > _height)
					IncreaseRow(new_height - _height);
				else if (_height > new_height)
					DecreaseRow(_height - new_height);

				if (new_width > _width)
					IncreaseColumn(new_width - _width);
				else if (new_width < _width)
					DecreaseColumn(_width - new_width);
			}

			//print the martix as string.
			void Print(std::function<std::string(const Element&)> trans_func) const
			{
				for (size_t y = 0; y < _height; y++)
				{
					for (size_t x = 0; x < _width; x++)
					{
						std::cout << trans_func(element(x, y));
						std::cout << " ";
					}
					std::cout << std::endl;
				}
				std::cout << std::endl;
			}
		};

		/*
		* RectangularArray offers a two-dimensional array.
		*
		* [T] is the type of element.
		* [_WIDTH] is the width of matrix.
		* [_HEIGHT] is the height of matrix.
		*/
		template<typename T, size_t _WIDTH, size_t _HEIGHT>
		class RectangularArray
		{
		private:
			using pointer = T*;
			using reference = T&;
			using const_reference = const T&;
			using Element = T;
			using Row = std::vector<pointer>;
			using Column = Row;
			using InitList = std::initializer_list<T>;
			using Iter = MatrixIter;

		private:

			Element _elements[_WIDTH][_HEIGHT];

		public:

			//return true if the coordinate is legal.
			inline bool is_legal_coordinate(size_t x, size_t y) const
			{
				return x < _WIDTH && y < _HEIGHT;
			}

			//return true if the coordinate is legal.
			inline bool is_legal_coordinate(UnsignedCoordinate coord) const
			{
				return is_legal_coordinate(coord.x, coord.y);
			}

			//constructor function with default elements.
			RectangularArray() = default;

			//constructor function with appointed elements.
			RectangularArray(Element elem)
			{
				for (auto coord : *this)
					set_element(coord, elem);
			}

			//get height, which is the number of rows. 
			inline constexpr size_t height() const
			{
				return _HEIGHT;
			}

			//get width, which is the number of columns.
			inline constexpr size_t width() const
			{
				return _WIDTH;
			}

			//get iterator begin
			Iter begin() const
			{
				return Iter({ 0, 0 }, _WIDTH, _HEIGHT);
			}

			//get iterator end
			Iter end() const
			{
				return Iter({ 0, _HEIGHT }, _WIDTH, _HEIGHT);
			}

			//get element
			inline const_reference element(size_t x, size_t y) const
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, !is_legal_coordinate(x, y), "out of row range.");
				return _elements[x][y];
			}

			//get element
			inline const_reference element(UnsignedCoordinate coord) const
			{
				return element(coord.x, coord.y);
			}

			//set element
			inline void set_element(size_t x, size_t y, const_reference elem)
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, !is_legal_coordinate(x, y), "out of row range.");
				_elements[x][y] = elem;
			}

			//set element
			inline void set_element(UnsignedCoordinate coord, const_reference elem)
			{
				set_element(coord.x, coord.y, elem);
			}

			//return the Row which include pointers to elements in the row.
			Row get_row(size_t row_index)
			{
				Row row(width());
				for (size_t i = 0; i < width(); i++)
					row[i] = &_elements[i][row_index];
				return row;
			}

			//return the Column which include pointers to elements in the column.
			Column get_column(size_t column_index)
			{
				Row column(height());
				for (size_t i = 0; i < height(); i++)
					column[i] = &_elements[column_index][i];
				return column;
			}

			//set row as same element.
			void set_row(size_t row_index, const_reference elem)
			{
				for (pointer elem_ptr : get_row(row_index))
					*elem_ptr = elem;
			}

			//set row by init list.
			void set_row(size_t row_index, InitList list)
			{
				size_t i = 0;
				for (const_reference elem : list)
				{
					if (i < get_row(row_index).size())
						*get_row(row_index)[i] = elem;
					else
						break;
					i++;
				}
			}

			//set column as same element.
			void set_column(size_t column_index, const_reference elem)
			{
				for (pointer elem_ptr : get_column(column_index))
					*elem_ptr = elem;
			}

			//set column bt init list.
			void set_column(size_t column_index, InitList list)
			{
				size_t i = 0;
				for (const_reference elem : list)
				{
					if (i < get_column(column_index).size())
						*get_column(column_index)[i] = elem;
					else
						break;
					i++;
				}
			}

			//return true if any element exist in the matrix.
			inline bool any(const_reference elem) const
			{
				for (auto coord : *this)
				{
					if (element(coord) == elem)
						return true;
				}
				return false;
			}

			//return true if none of the element exist in the matrix.
			inline bool none(const_reference elem) const
			{
				for (auto coord : *this)
				{
					if (element(coord) == elem)
						return false;
				}
				return true;
			}

			inline reference operator[](UnsignedCoordinate coord)
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, !is_legal_coordinate(coord.x, coord.y), "out of row range.");
				return _elements[coord.x][coord.y];
			}
		};

		/*
		* RandomPool is a container of elements which support to get element randomly by weigh.
		*
		* [T] is the type of element.
		* [is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename T, bool _is_debug = false>
		class RandomPool
		{
		private:
			using pointer = T*;
			using reference = T&;
			using Element = RandomPoolElement<T>;
			using Allocator = stl::LinearAllocator<Element, _is_debug>;

		private:

			Allocator	_ele_alloc;
			size_t		_accumulated_range;

			constexpr inline bool is_debug() const
			{
				return _is_debug;
			}

		public:
			//default constructor.
			RandomPool(size_t max_size) :
				_ele_alloc(max_size),
				_accumulated_range(0)
			{
			}

			//constructor with init list.
			RandomPool(size_t max_size, std::initializer_list<std::pair<size_t, T>> init_list) :
				_ele_alloc(max_size),
				_accumulated_range(0)
			{
				for (const std::pair<size_t, T>& pair : init_list)
				{
					add(pair.first, pair.second);
				}
			}

			//clear all elements.
			void clear()
			{
				_ele_alloc.flush();
				_accumulated_range = 0;
			}

			//add new element by copy.
			inline bool add(size_t weight, T data)
			{
				if (_ele_alloc.construct(weight, _accumulated_range, _accumulated_range + weight, data))
				{
					_accumulated_range += weight;
					return true;
				}
				return false;
			}

			//add new element by constructor.
			template<class... Types>
			inline bool add(size_t weight, Types&&... args)
			{
				if (_ele_alloc.construct(weight, _accumulated_range, _accumulated_range + weight, std::forward<Types>(args)...))
				{
					_accumulated_range += weight;
					return true;
				}
				return false;
			}

			//get chance that element[index] be selected.
			inline double get_chance(size_t index) const
			{
				if (index < _ele_alloc.size())
				{
					return double(_ele_alloc[index]->weight) / double(_accumulated_range);
				}
				return 0.0;
			}

			//get element reference by index.
			inline const reference get_element(size_t index) const
			{
				return _ele_alloc[index]->data;
			}

			//get weight of element by index.
			inline size_t get_weight(size_t index) const
			{
				if (index < _ele_alloc.size())
				{
					return _ele_alloc[index]->weight;
				}
				return 0;
			}

			//get random element.
			inline const reference random() const
			{
				GADT_CHECK_WARNING(is_debug(), size() == 0, "random pool is empty.");
				size_t rnd = rand() % _accumulated_range;
				for (size_t i = 0; i < size(); i++)
				{
					if (rnd >= _ele_alloc[i]->left && rnd < _ele_alloc[i]->right)
					{
						return _ele_alloc[i]->data;
					}
				}
				GADT_CHECK_WARNING(is_debug(), true, "unsuccessful random pick up.");
				return _ele_alloc[0]->data;
			}

			//get the size of the element.
			inline size_t size() const
			{
				return _ele_alloc.size();
			}

			const reference operator[](size_t index)
			{
				return get_element(index);
			}
		};
	}
}
