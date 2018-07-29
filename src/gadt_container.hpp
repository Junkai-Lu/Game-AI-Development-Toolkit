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
#include "gadt_log.hpp"

#pragma once

#ifdef GADT_WARNING
	#define GADT_DYNAMIC_ARRAY_DEBUG_INFO
#endif

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
		template<typename T>
		class List
		{
		public:
			using Node = ListNode<T>;
			using Allocator = gadt::stl::StackAllocator<Node, GADT_STL_ENABLE_WARNING>;
			using node_pointer = Node*;

		private:
			const bool   _private_allocator;
			Allocator&	 _allocator;
			node_pointer _first_node;
			node_pointer _last_node;
			node_pointer _iterator;
			size_t       _size;

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
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, _last_node == nullptr, "no element in the back of link list");
				node_pointer temp = _last_node;
				_last_node = _last_node->prev_node();
				_last_node->set_next_node(nullptr);
				_allocator.destory(temp);
			}

			void pop_front()
			{
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, _first_node == nullptr, "no element in the front of link list");
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
		* MatrixIter is the iterator of matrix containers which include StaticMatrix and DynamicMatrix.
		*/
		class MatrixIter
		{
		private:
			UPoint _point;
			const size_t _width;
			const size_t _height;

		public:
			MatrixIter(UPoint point, size_t width, size_t height) :
				_point(point),
				_width(width),
				_height(height)
			{
			}

			bool operator!=(const MatrixIter& iter) const
			{
				return _point != iter._point;
			}

			void operator++()
			{
				_point.x++;
				if (_point.x >= _width)
				{
					_point.x = 0;
					_point.y++;
				}
			}

			UPoint operator* ()
			{
				return _point;
			}
		};

		//Declear Static Matrix.
		template<typename T, size_t _WIDTH, size_t _HEIGHT, typename std::enable_if<(_WIDTH > 0 && _HEIGHT > 0), int>::type = 0>
		class StaticMatrix;

		/*
		* DynamicMatrix is a flexiable matrix.
		*
		* [T] is the type of element.
		*/
		template<typename T>
		class DynamicMatrix
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

		public:

			using ElementToJsonFunc = std::function<json11::Json(const_reference)>;
			using ElementToStringFunc = std::function<std::string(const_reference)>;
			using StringToElementFunc = std::function<Element(const std::string&)>;
			using JsonToElementFunc = std::function<Element(const json11::Json)>;

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

		public:

			//return true if the Point is legal
			inline bool is_legal_point(size_t x, size_t y) const
			{
				return (x < _width) && (y < _height);
			}

			//return true if the Point is legal
			inline bool is_legal_point(UPoint point) const
			{
				return is_legal_point(point.x, point.y);
			}

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
			inline const_reference element(UPoint point) const 
			{
				return element(point.x, point.y);
			}

			//get element
			inline const_reference element(size_t x, size_t y) const 
			{
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, !is_legal_point(x,y), "out of row range.");
				return (_elements[x])[y];
			}

			//get row by index.
			inline const Row& get_row(size_t index) const
			{
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, index >= _height, "out of row range.");
				return _row[index];
			}

			//get column by index.
			inline const Column& get_column(size_t index) const
			{
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, index >= _width, "out of column range.");
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
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, !is_legal_point(x, y), "out of range.");
				(_elements[x])[y] = elem;
			}

			//set element.
			inline void set_element(const_reference elem, UPoint point)
			{
				set_element(elem, point.x, point.y);
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
						return;
					i++;
				}
				for (; i < _width; i++)
					*get_row(row_index)[i] = Element();
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
				for (; i < _height; i++)
					*get_column(column_index)[i] = Element();
			}

			//return true if element exist.
			inline bool any(const_reference elem) const
			{
				for (auto point : *this)
				{
					if (element(point) == elem)
						return true;
				}
				return false;
			}

			//return true if element do not exist.
			inline bool none(const_reference elem) const
			{
				for (auto point : *this)
				{
					if (element(point) == elem)
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

			//get element by point.
			inline reference operator[](UPoint point)
			{
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, !is_legal_point(point.x, point.y), "out of row range.");
				return (_elements[point.x])[point.y];
			}

		public:

			//default constructor function.
			DynamicMatrix():
				_width(0),
				_height(0),
				_elements(),
				_column(),
				_row()
			{
			}

			//constructor function
			DynamicMatrix(size_t width, size_t height, const_reference elem = Element()) :
				_width(width),
				_height(height),
				_elements(width, std::vector<T>(height, elem))
			{
				init_column_and_row();
			}

			//constructor function with initializer list.
			template<typename ParamType>
			DynamicMatrix(size_t column_size, size_t row_size, std::initializer_list<std::initializer_list<ParamType>> list) :
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
							set_element(Element(value), x, y);
						x++;
					}
					for (; x < _width; x++)
						set_element(Element(), x, y);
					y++;
					if (y >= _height)
						return;
				}
				for (; y < _height; y++)
					for(size_t  x = 0;x < _width; x++)
						set_element(Element(), x, y);
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

			//resize the matrix.
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

			//print the matrix as string.
			void Print(ElementToStringFunc ElemToString) const
			{
				for (size_t y = 0; y < _height; y++)
				{
					for (size_t x = 0; x < _width; x++)
					{
						std::cout << ElemToString(element(x, y));
						std::cout << " ";
					}
					std::cout << std::endl;
				}
				std::cout << std::endl;
			}

			//convert to JSON object in which each element in the matrix need to be convert to a JSON object.
			json11::Json ConvertToJsonObj(ElementToJsonFunc ElemToJson) const
			{
				std::vector<json11::Json> json;
				for (size_t row_index = 0; row_index < _height; row_index++)
				{
					std::vector<json11::Json> row_json;
					for (size_t column_index = 0; column_index < _width; column_index++)
						row_json.push_back(ElemToJson(element(column_index, row_index)));
					json.push_back(json11::Json::array(row_json));
				}
				return json11::Json::array(json);
			}

			//convert to JSON object in which each element in the matrix need to be convert to a string.
			json11::Json ConvertToJsonObj(ElementToStringFunc ElemToString) const
			{
				std::function<json11::Json(const_reference)> ElemaToJson = [&](const_reference value)->json11::Json {
					return json11::Json{ ElemToString(value) };
				};
				return ConvertToJsonObj(ElemaToJson);
			}

			//load from JSON string. return true if load success.
			bool LoadFromJson(std::string json_str, JsonToElementFunc JsonToElem)
			{
				std::string err;
				json11::Json json = json11::Json::parse(json_str, err);
				if (err != "")
					return false;
				if (json.is_array())
				{
					if (json.array_items().size() > 0)
					{
						size_t row_count = json.array_items().size();
						if (json.array_items()[0].is_array())
						{
							if (json.array_items()[0].array_items().size() > 0)
							{
								size_t column_count = json.array_items()[0].array_items().size();
								DynamicMatrix<T> temp(column_count, row_count);
								for (size_t row_index = 0; row_index < row_count; row_index++)
								{
									const json11::Json& row_json = json.array_items()[row_index];

									//return false if the json object is not array.
									if (row_json.is_array() == false)
										return false;
									//return false if the size of the json array is incorrect. 
									if (row_json.array_items().size() != column_count)
										return false;
									for (size_t column_index = 0; column_index < column_count; column_index++)
									{
										const json11::Json value_json = row_json.array_items()[column_index];
										temp.set_element(JsonToElem(value_json), column_index, row_index);
									}
								}
								*this = temp;
								return true;
							}
						}
					}
					else
					{
						Resize(0, 0);//empty matrix.
						return true;
					}
				}
				return false;
			}

			//load from JSON string. return true if load success.
			bool LoadFromJson(std::string json_str, StringToElementFunc StringToElem)
			{
				std::string err;
				json11::Json json = json11::Json::parse(json_str, err);
				if (err != "")
					return false;
				if (json.is_array())
				{
					if (json.array_items().size() > 0)
					{
						size_t row_count = json.array_items().size();
						if (json.array_items()[0].is_array())
						{
							if (json.array_items()[0].array_items().size() > 0)
							{
								size_t column_count = json.array_items()[0].array_items().size();
								DynamicMatrix<T> temp(column_count, row_count);
								for (size_t row_index = 0; row_index < row_count; row_index++)
								{
									const json11::Json& row_json = json.array_items()[row_index];
									if (row_json.is_array() == false)
										return false;//return false if the json object is not array.
									if (row_json.array_items().size() != column_count)
										return false;//return false if the size of the json array is incorrect. 
									for (size_t column_index = 0; column_index < column_count; column_index++)
									{
										const json11::Json value_json = row_json.array_items()[column_index];
										if (value_json.is_string() == false)
											return false;
										temp.set_element(StringToElem(value_json.string_value()), column_index, row_index);
									}
								}
								*this = temp;
								return true;
							}
						}
					}
					else
					{
						Resize(0, 0);//empty matrix.
						return true;
					}
				}
				return false;
			}
		};

		/*
		* StaticMatrix offers a two-dimensional array.
		*
		* [T] is the type of element.
		* [_WIDTH] is the width of matrix.
		* [_HEIGHT] is the height of matrix.
		*/
		template<typename T, size_t _WIDTH, size_t _HEIGHT, typename std::enable_if<(_WIDTH > 0 && _HEIGHT > 0), int>::type>
		class StaticMatrix
		{
		private:
			using pointer = T*;
			using reference = T&;
			using const_reference = const T&;
			using Element = T;
			using InitList = std::initializer_list<T>;
			using Iter = MatrixIter;

		public:

			using Row = StaticMatrix<T, _WIDTH, 1>;
			using Column = StaticMatrix<T, 1, _HEIGHT>;
			using ElementToJsonFunc = std::function<json11::Json(const_reference)>;
			using ElementToStringFunc = std::function<std::string(const_reference)>;
			using StringToElementFunc = std::function<Element(const std::string&)>;
			using JsonToElementFunc = std::function<Element(const json11::Json)>;

		private:

			Element _elements[_WIDTH * _HEIGHT];

		private:

			inline size_t get_index(size_t x, size_t y) const
			{
				return ( x * _WIDTH ) + y;
			}

		public:

			//return true if the Point is legal.
			inline bool is_legal_point(size_t x, size_t y) const
			{
				return (x < _WIDTH) && (y < _HEIGHT);
			}

			//return true if the Point is legal.
			inline bool is_legal_point(UPoint point) const
			{
				return is_legal_point(point.x, point.y);
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

			//return the total number of elements in this matrix.
			inline constexpr size_t element_count() const
			{
				return _WIDTH * _HEIGHT;
			}

			//get iterator begin
			inline Iter begin() const
			{
				return Iter({ 0, 0 }, _WIDTH, _HEIGHT);
			}

			//get iterator end
			inline Iter end() const
			{
				return Iter({ 0, _HEIGHT }, _WIDTH, _HEIGHT);
			}

			//get element
			inline const_reference element(size_t x, size_t y) const
			{
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, !is_legal_point(x, y), "out of row range.");
				return _elements[get_index(x, y)];
			}

			//get element
			inline const_reference element(UPoint point) const
			{
				return element(point.x, point.y);
			}

			//set element
			inline void set_element(const_reference elem, size_t x, size_t y)
			{
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, !is_legal_point(x, y), "out of row range.");
				_elements[get_index(x, y)] = elem;
			}

			//set element
			inline void set_element(const_reference elem, UPoint point)
			{
				set_element(elem, point.x, point.y);
			}

			//return true if any element exist in the matrix.
			inline bool any(const_reference elem) const
			{
				for (auto point : *this)
				{
					if (element(point) == elem)
						return true;
				}
				return false;
			}

			//return true if none of the element exist in the matrix.
			inline bool none(const_reference elem) const
			{
				for (auto point : *this)
				{
					if (element(point) == elem)
						return false;
				}
				return true;
			}

			inline reference operator[](UPoint point)
			{
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, !is_legal_point(point.x, point.y), "out of row range.");
				return _elements[get_index(point.x, point.y)];
			}

		public:

			//constructor function with default elements.
			StaticMatrix()
			{
				for (size_t i = 0; i < element_count(); i++)
					_elements[i] = Element();
			}

			//constructor function with appointed elements.
			StaticMatrix(Element elem)
			{
				for (size_t i = 0; i < element_count(); i++)
					_elements[i] = elem;
			}

			//return the Row which include pointers to elements in the row.
			Row get_row(size_t row_index)
			{
				Row row;
				for (size_t i = 0; i < width(); i++)
					row.set_element(element(i, row_index), i, 1);
				return row;
			}

			//return the Column which include pointers to elements in the column.
			Column get_column(size_t column_index)
			{
				Row column;
				for (size_t i = 0; i < height(); i++)
					column.set_element(element(column_index, i), 1, i);
				return column;
			}

			//set row as same element.
			void set_row(size_t row_index, const_reference elem)
			{
				for (size_t i = 0; i < _WIDTH; i++)
					set_element(elem, i, row_index);
			}

			//set row by init list.
			void set_row(size_t row_index, InitList list)
			{
				size_t i = 0;
				for (const_reference elem : list)
				{
					if (i < _WIDTH)
						set_element(elem, i, row_index);//set elements in list.
					else
						return;
					i++;
				}
				for (; i < _WIDTH; i++)
					set_element(Element(), i, row_index);//set default.
			}

			//set column as same element.
			void set_column(size_t column_index, const_reference elem)
			{
				for (size_t i = 0; i < _WIDTH; i++)
					set_element(elem, column_index, i);
			}

			//set column bt init list.
			void set_column(size_t column_index, InitList list)
			{
				size_t i = 0;
				for (const_reference elem : list)
				{
					if (i < _WIDTH)
						set_element(elem, column_index, i);//set elements in list.
					else
						return;
					i++;
				}
				for (; i < _WIDTH; i++)
					set_element(Element(), column_index, i);//set default.
			}

			//print the matrix as string.
			void Print(ElementToStringFunc ElemToString) const
			{
				for (size_t y = 0; y < _HEIGHT; y++)
				{
					for (size_t x = 0; x < _WIDTH; x++)
					{
						std::cout << ElemToString(element(x, y));
						std::cout << " ";
					}
					std::cout << std::endl;
				}
				std::cout << std::endl;
			}

			//convert to string format.
			std::string ConvertToString(ElementToStringFunc ElemToString, std::string delimiter = console::DELIMITER_STR_SPACE) const
			{
				std::stringstream ss;
				for (size_t y = 0; y < height(); y++)
				{
					for (size_t x = 0; x < width(); x++)
					{
						ss << ElemToString(element(x, y)) << delimiter;
					}
					ss << std::endl;
				}
				return ss.str();
			}

			//convert to JSON object in which each element in the matrix need to be convert to a JSON object.
			json11::Json ConvertToJsonObj(ElementToJsonFunc ElemToJson) const
			{
				std::vector<json11::Json> json;
				for (size_t row_index = 0; row_index < _HEIGHT; row_index++)
				{
					std::vector<json11::Json> row_json;
					for (size_t column_index = 0; column_index < _WIDTH; column_index++)
						row_json.push_back(ElemToJson(element(column_index, row_index)));
					json.push_back(json11::Json::array(row_json));
				}
				return json11::Json::array(json);
			}

			//convert to JSON object in which each element in the matrix need to be convert to a string.
			json11::Json ConvertToJsonObj(ElementToStringFunc ElemToString) const
			{
				std::function<json11::Json(const_reference)> ElemaToJson = [&](const_reference value)->json11::Json {
					return json11::Json{ ElemToString(value) };
				};
				return ConvertToJsonObj(ElemaToJson);
			}

			//load from JSON string. return true if load success.
			bool LoadFromJson(std::string json_str, JsonToElementFunc JsonToElem)
			{
				std::string err;
				json11::Json json = json11::Json::parse(json_str, err);
				if (err != "")
					return false;
				if (json.is_array())
				{
					if (json.array_items().size() == _HEIGHT)
					{
						if (json.array_items()[0].is_array())
						{
							if (json.array_items()[0].array_items().size() > 0)
							{
								StaticMatrix<T, _WIDTH, _HEIGHT> temp;
								for (size_t row_index = 0; row_index < _HEIGHT; row_index++)
								{
									const json11::Json& row_json = json.array_items()[row_index];
									if (row_json.is_array() == false)
										return false;//return false if the json object is not array.
									if (row_json.array_items().size() != _WIDTH)
										return false;//return false if the size of the json array is incorrect. 
									for (size_t column_index = 0; column_index < _WIDTH; column_index++)
									{
										const json11::Json value_json = row_json.array_items()[column_index];
										temp.set_element(JsonToElem(value_json), column_index, row_index);
									}
								}
								*this = temp;
								return true;
							}
						}
					}
				}
				return false;
			}

			//load from JSON string. return true if load success.
			bool LoadFromJson(std::string json_str, StringToElementFunc StringToElem)
			{
				std::string err;
				json11::Json json = json11::Json::parse(json_str, err);
				if (err != "")
					return false;
				if (json.is_array())
				{
					if (json.array_items().size() == _HEIGHT)
					{
						if (json.array_items()[0].is_array())
						{
							if (json.array_items()[0].array_items().size() > 0)
							{
								StaticMatrix<T, _WIDTH, _HEIGHT> temp;
								for (size_t row_index = 0; row_index < _HEIGHT; row_index++)
								{
									const json11::Json& row_json = json.array_items()[row_index];
									if (row_json.is_array() == false)
										return false; //return false if the json object is not array.
									if (row_json.array_items().size() != _WIDTH)
										return false;//return false if the size of the json array is incorrect. 
									for (size_t column_index = 0; column_index < _WIDTH; column_index++)
									{
										const json11::Json value_json = row_json.array_items()[column_index];
										if (value_json.is_string() == false)
											return false;
										temp.set_element(StringToElem(value_json.string_value()), column_index, row_index);
									}
								}
								*this = temp;
								return true;
							}
						}
					}
				}
				return false;
			}

			//convert to dynamic matrix.
			DynamicMatrix<T> ToDynamic()
			{
				DynamicMatrix<T> matrix(_WIDTH, _HEIGHT, element(0, 0));
				for (auto point : *this)
					matrix.set_element(element(point), point);
				return matrix;
			}

			//return the submatrix of this matrix.
			template<size_t AREA_WIDTH, size_t AREA_HEIGHT, typename std::enable_if<((AREA_WIDTH <= _WIDTH) && (AREA_HEIGHT <= _HEIGHT)), int>::type = 0>
			StaticMatrix<T, AREA_WIDTH, AREA_HEIGHT> SubMatrix(UPoint point) const
			{
				StaticMatrix<T, AREA_WIDTH, AREA_HEIGHT> submatrix;
				for (auto sub_point : submatrix)
				{
					auto pos = point + sub_point;
					if (is_legal_point(pos))
						submatrix[sub_point] = element(pos);
				}
				return submatrix;
			}

			//return the submatrix of this matrix.
			template<size_t AREA_WIDTH, size_t AREA_HEIGHT, typename std::enable_if<((AREA_WIDTH <= _WIDTH) && (AREA_HEIGHT <= _HEIGHT)), int>::type = 0>
			StaticMatrix<T, AREA_WIDTH, AREA_HEIGHT> SubMatrix(size_t x, size_t y) const
			{
				return SubMatrix<AREA_WIDTH, AREA_HEIGHT>(UPoint{ x,y });
			}
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
		*/
		template<typename T>
		class RandomPool
		{
		private:
			using pointer = T*;
			using reference = T&;
			using Element = RandomPoolElement<T>;
			using Allocator = stl::LinearAllocator<Element, GADT_STL_ENABLE_WARNING>;

		private:

			Allocator	_ele_alloc;
			size_t		_accumulated_range;

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
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, size() == 0, "random pool is empty.");
				size_t rnd = rand() % _accumulated_range;
				for (size_t i = 0; i < size(); i++)
				{
					if (rnd >= _ele_alloc[i]->left && rnd < _ele_alloc[i]->right)
					{
						return _ele_alloc[i]->data;
					}
				}
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, true, "unsuccessful random pick up.");
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

		/*
		* StaticArray is a array which allow user to dynamiclly allocate its size.
		*
		* [T] is the type of element.
		*/
		template<typename T>
		class DynamicArray final
		{
		private:
			using pointer = T * ;
			using reference = T & ;

			static const size_t		_size = sizeof(T);
			size_t					_count;
			pointer					_fir_element;
			size_t					_length;

#ifdef GADT_DYNAMIC_ARRAY_DEBUG_INFO
			std::vector<pointer>	_pointers;
#endif

		private:
			//allocate memory
			inline void alloc_memory(size_t count)
			{
				_fir_element = reinterpret_cast<T*>(calloc(count, _size));
#ifdef GADT_DYNAMIC_ARRAY_DEBUG_INFO
				for (size_t i = 0; i < count; i++)
					_pointers.push_back(_fir_element + i);
#endif
			}

			//delete memory
			inline void delete_memory()
			{
				//flush();
				::free(_fir_element);
				_fir_element = nullptr;
				_length = 0;
			}

		public:

			//default constructor
			DynamicArray():
				_count(0),
				_fir_element(nullptr),
				_length(0)
			{
			}

			//constructor function with allocation.
			DynamicArray(size_t count) :
				_count(count),
				_fir_element(nullptr),
				_length(0)
			{
				alloc_memory(count);
			}

			//copy constructor function.
			DynamicArray(const DynamicArray& target) :
				_count(target._count),
				_fir_element(nullptr),
				_length(0)
			{
				if (target._fir_element != nullptr)
				{
					alloc_memory(_count);
					::memcpy(_fir_element, target._fir_element, _size * _count);
					_length = target._length;
				}
			}

			//destructor function.
			~DynamicArray()
			{
				delete_memory();
			}

			//allocate memory for uninitialized container.
			bool allocate(size_t count)
			{
				if (_fir_element == nullptr)
				{
					alloc_memory(count);
					_count = count;
					return true;
				}
				return false;
			}

			//free space by ptr, return true if free successfully.
			inline bool pop_back()
			{
				if (_length > 0)
				{
					_length--;
					return true;
				}
				return false;
			}

			//copy source object to a empty space and return the pointer, return false if there are not available space.
			bool push_back(const reference elem)//T* constructor(const T& source)
			{
				if (is_full() == false)
				{
					pointer ptr = _fir_element + _length;
					_length++;
					*ptr = elem;
					return true;
				}
				return false;
			}

			//copy source object to a empty space and return the pointer, return false if there are not available space.
			bool push_back(T&& elem)//T* constructor(const T& source)
			{
				if (is_full() == false)
				{
					pointer ptr = _fir_element + _length;
					_length++;
					*ptr = std::move(elem);
					return true;
				}
				return false;
			}

			//get first element.
			inline reference element(size_t index)
			{
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, index >= _length, "out of range");
				return *(_fir_element + index);
			}

			inline const reference at(size_t index) const
			{
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, index >= _length, "out of range");
				return *(_fir_element + index);
			}

			//get the element in front.
			inline const reference front() const
			{
				return at(0);
			}

			//get the element in back.
			inline const reference back() const
			{
				return at(_length - 1);
			}

			//swap element.
			inline void swap(size_t fir_index, size_t sec_index)
			{
				T temp = element(fir_index);
				element(fir_index) = element(sec_index);
				element(sec_index) = temp;
			}

			//total size of alloc.
			inline size_t max_size() const
			{
				return _count;
			}

			//remain size in the alloc.
			inline size_t remain_size() const
			{
				return _count - _length;
			}

			//return size of the vector.
			inline size_t size() const
			{
				return _length;
			}

			//return true if there is not available space in this vector.
			inline bool is_full() const
			{
				return _count == _length;
			}

			//return true if this vector is empty.
			inline bool is_empty() const
			{
				return _length == 0;
			}

			//flush all datas.
			inline void flush()
			{
				while (pop_back()) {}
			}

			//pointer of first element.
			pointer begin() const
			{
				return _fir_element;
			}

			//pointer of last element.
			pointer end() const
			{
				return _fir_element + _length;
			}

			reference operator[](size_t index)
			{
				return element(index);
			}


		};

		/*
		* StaticArray is a array which allow user to staticly allocate its size.
		*
		* [T] is the type of element.
		* [_MAX_SIZE] is the size of the array.
		*/
		template<typename T, size_t _MAX_SIZE>
		class StaticArray final
		{
		private:
			using pointer = T *;
			using reference = T &;

			T _elems[_MAX_SIZE];
			size_t	_size;

		public:

			//default constructor
			StaticArray() :
				_size(0)
			{
			}

			StaticArray(const T& default_value)
			{
				for (size_t i = 0; i < _MAX_SIZE; i++)
				{
					_elems[i] = default_value;
				}
			}

			//copy constructor function.
			StaticArray(const StaticArray& target) :
				_size(0)
			{
				::memcpy(_elems, target._elems, _size * _MAX_SIZE);
				_size = target._size;
			}

			//free space by ptr, return true if free successfully.
			inline bool pop_back()
			{
				if (_size > 0)
				{
					_size--;
					return true;
				}
				return false;
			}

			//copy source object to a empty space and return the pointer, return false if there are not available space.
			bool push_back(const reference elem)//T* constructor(const T& source)
			{
				if (is_full() == false)
				{
					_elems[_size] = elem;
					_size++;
					return true;
				}
				return false;
			}

			//copy source object to a empty space and return the pointer, return false if there are not available space.
			bool push_back(T&& elem)//T* constructor(const T& source)
			{
				if (is_full() == false)
				{
					_elems[_size] = std::move(elem);
					_size++;
					return true;
				}
				return false;
			}

			//get first element.
			inline reference element(size_t index)
			{
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, index >= _size, "out of range");
				return _elems[index];
			}

			//get the element in front.
			inline reference front()
			{
				return element(0);
			}

			//get the element in back.
			inline reference back()
			{
				return element(_size - 1);
			}

			//swap element.
			inline void swap(size_t fir_index, size_t sec_index)
			{
				T temp = element(fir_index);
				element(fir_index) = element(sec_index);
				element(sec_index) = temp;
			}

			//total size of alloc.
			inline size_t max_size() const
			{
				return _MAX_SIZE;
			}

			//remain size in the alloc.
			inline size_t remain_size() const
			{
				return max_size() - _size;
			}

			//return size of the vector.
			inline size_t size() const
			{
				return _size;
			}

			//return true if there is not available space in this vector.
			inline bool is_full() const
			{
				return _MAX_SIZE == _size;
			}

			//return true if this vector is empty.
			inline bool is_empty() const
			{
				return _size == 0;
			}

			//pointer of first element.
			inline pointer begin()
			{
				return &front();
			}

			//pointer of last element.
			inline pointer end()
			{
				return &(_elems[0]) + _MAX_SIZE;
			}

			inline reference operator[](size_t index)
			{
				return element(index);
			}
		};

		/*
		* ElementPool allows user to push elements and get them randomly.
		*
		* [T] is the type of element.
		* [_MAX_SIZE] is the size of the array.
		*/
		template<typename T, size_t _MAX_SIZE>
		class ElementPool final
		{
		private:
			StaticArray<T, _MAX_SIZE> _elements;

		public:

			//push new element into pool
			void push(const T& elem)
			{
				_elements.push_back(elem);
			}

			//get a random element and pop it from the pool.
			T random_pop()
			{
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, is_empty(), "overflow");
				size_t rnd = rand() % size();
				T temp = _elements[rnd];
				_elements.swap(rnd, size() - 1);
				_elements.pop_back();
				return temp;
			}

			//get a random element.
			const T& random() const
			{
				GADT_WARNING_IF(GADT_STL_ENABLE_WARNING, is_empty(), "overflow");
				size_t rnd = rand() % size();
				return _elements[rnd];
			}

			//current size of the pool
			size_t size() const
			{
				return _elements.size();
			}

			//max size of the pool.
			size_t max_size() const
			{
				return _elements.max_size();
			}

			//return true if the pool is full.
			bool is_full() const
			{
				return _elements.is_full();
			}

			//return true if the pool is empty.
			bool is_empty() const
			{
				return _elements.is_empty();
			}

		public:

			ElementPool():
				_elements()
			{
			}

			ElementPool(const ElementPool& target) :
				_elements(target._elements)
			{
			}

			ElementPool(std::initializer_list<T> elems)
			{
				for (auto elem : elems)
					push(elem);
			}
		};
	}
}
