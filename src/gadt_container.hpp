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

			//get info of the random pool
			std::string info() const
			{
				table::ConsoleTable tb(3, _ele_alloc.size() + 1);
				tb.set_cell_in_row(0, { { "index" },{ "weight" },{ "range" } });
				tb.set_width({ 6,6,10 });
				tb.enable_title({ "random pool" });
				for (size_t i = 0; i < _ele_alloc.size(); i++)
				{
					tb.set_cell_in_row(i + 1, {
						{ ToString(i) },
						{ ToString(get_weight(i)) },
						{ _ele_alloc.element(i)->range() }
					});
				}
				return tb.output_string();
			}

			const reference operator[](size_t index)
			{
				return get_element(index);
			}
		};

		/*
		* MatrixArrayIter is the iterator of matrix container.
		*/
		class MatrixIter
		{
		private:
			Coordinate _coord;
			const size_t _width;
			const size_t _height;

		public:
			MatrixIter(Coordinate coord, size_t width, size_t height) :
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

			Coordinate operator* ()
			{
				return _coord;
			}
		};

		/*
		* ElementMatrix is a flexiable matrix.
		*
		* [T] is the type of element.
		*/
		template<typename T>
		class ElementMatrix
		{
		public:
			using pointer = T*;
			using reference = T&;
			using const_reference = const T&;
			using Element = T;
			using ElementSet = std::vector<std::vector<T>>;
			using Row = std::vector<pointer>;
			using Column = Row;
			using InitList = std::initializer_list<T>;

		private:

			const size_t		_number_of_columns;
			const size_t		_number_of_row;
			ElementSet			_elements;
			std::vector<Column> _column;
			std::vector<Row>	_row;

		private:
			void init_column_and_row()
			{
				//init columns
				for (size_t column = 0; column < _number_of_columns; column++)
				{
					Column temp;
					for (size_t row = 0; row < _number_of_row; row++)
					{
						temp.push_back(&((_elements[column])[row]));
					}
					_column.push_back(temp);
				}

				//init rows
				for (size_t row = 0; row < _number_of_row; row++)
				{
					Row temp;
					for (size_t column = 0; column < _number_of_columns; column++)
					{
						temp.push_back(&((_elements[column])[row]));
					}
					_row.push_back(temp);
				}
			}

			inline bool is_legal_coordinate(size_t x, size_t y)
			{
				return x < _number_of_columns && y < _number_of_row;
			}

		public:

			//constructor function
			ElementMatrix(size_t column_size, size_t row_size, const_reference elem = Element()) :
				_number_of_columns(column_size),
				_number_of_row(row_size),
				_elements(column_size, std::vector<T>(row_size, elem))
			{
				init_column_and_row();
			}

			//constructor function with initializer list.
			ElementMatrix(size_t column_size, size_t row_size, std::initializer_list<InitList> list) :
				_number_of_columns(column_size),
				_number_of_row(row_size),
				_elements()
			{
				init_column_and_row();
				size_t y = 0;
				for (auto row : list)
				{
					size_t x = 0;
					for (auto value : row)
					{
						if (x < _number_of_columns && y < _number_of_row)
						{
							((_elements[x])[y]).str = value;
						}
						x++;
					}
					y++;
				}
			}

			//get number of rows.
			inline size_t number_of_rows() const
			{
				return _number_of_row;
			}

			//get number of columns.
			inline size_t number_of_columns() const
			{
				return _number_of_columns;
			}

			//get element
			inline const Element& element(Coordinate coord)
			{
				return element(coord.x, coord.y);
			}

			//get element
			inline const Element& element(size_t column, size_t row)
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, !is_legal_coordinate(column,row), "out of row range.");
				return (_elements[column])[row];
			}

			//get row by index.
			inline const Row& get_row(size_t index)
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, index >= _number_of_row, "out of row range.");
				return _row[index];
			}

			//get column by index.
			inline const Column& get_column(size_t index)
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, index >= _number_of_columns, "out of column range.");
				return _column[index];
			}

			//set element.
			void set_element(const reference elem)
			{
				for (std::vector<Element>& row : _elements)
					for (Element& e : row)
						e = elem;
			}

			//set element.
			void set_element(const reference elem, size_t column, size_t row)
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, !is_legal_coordinate(column, row), "out of range.");
				(_elements[column])[row] = elem;
			}

			//set element.
			void set_element(const reference elem, Coordinate coord)
			{
				set_element(coord.x, coord.y);
			}

			void set_row(size_t row, const reference elem)
			{
				for (pointer elem_ptr : get_row(row))
					*elem_ptr = elem;
			}

			void set_row(size_t row, InitList list)
			{
				size_t i = 0;
				for (const reference elem : list)
				{
					if (i < get_row(row).size())
						*get_row(row)[i] = elem;
					else
						break;
					i++;
				}
			}

			void set_column(size_t column, const reference elem)
			{
				for (pointer elem_ptr : get_column(column))
					*elem_ptr = elem;
			}

			void set_column(size_t column, InitList list)
			{
				size_t i = 0;
				for (const reference elem : list)
				{
					if (i < get_column(column).size())
						*get_column(column)[i] = elem;
					else
						break;
					i++;
				}
			}
		};

		/*
		* MatrixArray offers a two-dimensional array.
		*
		* [T] is the type of element.
		* [_WIDTH] is the width of matrix.
		* [_HEIGHT] is the height of matrix.
		*/
		template<typename T, size_t _WIDTH, size_t _HEIGHT>
		class MatrixArray
		{
			using pointer = T*;
			using reference = T&;
			using const_reference = const T&;
			using Element = T;
			using InitList = std::initializer_list<T>;
			using Iter = MatrixIter;

		private:

			Element _elements[_WIDTH][_HEIGHT];

		public:

			inline bool is_legal_coordinate(size_t x, size_t y) const
			{
				return x < _WIDTH && y < _HEIGHT;
			}

			inline bool is_legal_coordinate(Coordinate coord) const
			{
				return is_legal_coordinate(coord.x, coord.y);
			}

			MatrixArray() = default;

			MatrixArray(Element elem)
			{
				for (auto coord : *this)
					set_element(coord, elem);
			}

			Iter begin() const
			{
				return Iter({ 0, 0 }, _WIDTH, _HEIGHT);
			}

			Iter end() const
			{
				return Iter({ 0, _HEIGHT }, _WIDTH, _HEIGHT);
			}

			inline const_reference element(size_t x, size_t y) const
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, !is_legal_coordinate(x, y), "out of row range.");
				return _elements[x][y];
			}

			inline const_reference element(Coordinate coord) const
			{
				return element(coord.x, coord.y);
			}

			inline void set_element(size_t x, size_t y, const_reference elem)
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, !is_legal_coordinate(x, y), "out of row range.");
				_elements[x][y] = elem;
			}

			inline void set_element(Coordinate coord, const_reference elem)
			{
				set_element(coord.x, coord.y, elem);
			}

			inline bool any(const_reference elem) const
			{
				for (auto coord : *this)
				{
					if (element(coord) == elem)
						return true;
				}
				return false;
			}

			inline bool none(const_reference elem) const
			{
				for (auto coord : *this)
				{
					if (element(coord) == elem)
						return false;
				}
				return true;
			}

			inline reference operator[](Coordinate coord)
			{
				GADT_CHECK_WARNING(GADT_STL_ENABLE_WARNING, !is_legal_coordinate(coord.x, coord.y), "out of row range.");
				return _elements[coord.x][coord.y];
			}
		};
	}
}
