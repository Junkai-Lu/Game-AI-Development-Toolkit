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
	#define GADT_CHECK_WARNING(warning_condition, reason) gadt::console::WarningCheck(warning_condition, reason, __FILE__, __LINE__, __FUNCTION__);
#else
	#define GADT_CHECK_WARNING(warning_condition, reason)
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
			std::ostream& _os;
			ConsoleColor _color;

			//change colors by setting using winapi in windows or output string in linux.
			static std::string change_color(ConsoleColor color);
		public:
			costream(std::ostream& os) : _os(os),_color(DEFAULT) {}
			costream& operator<<(ConsoleColor color)
			{
				_color = color;
				_os << change_color(color);
				return *this;
			}

			template<typename datatype>
			inline costream& operator<<(datatype d)
			{
				_os << d;
				return *this;
			}

			template <typename t_data>
			void print(t_data data, ConsoleColor color);
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
		void Cprintf(std::string	data,	ConsoleColor color);
		void Cprintf(double			data,	ConsoleColor color);
		void Cprintf(int			data,	ConsoleColor color);
		void Cprintf(char			data,	ConsoleColor color);

		//show error in terminal.
		void ShowError(std::string reason);

		//show message in terminal.
		void ShowMessage(std::string message, bool show_msg = true);

		//if 'condition' is true that report detail.
		void WarningCheck(bool condition, std::string reason, std::string file, int line, std::string function);

		//system pause.
		void SystemPause();

		//system clear
		void SystemClear();

		
	}

	//a golbal color ostream 
	extern console::costream ccout;

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
		template<int begin_index, size_t length, typename Tdata>
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

	namespace stl
	{
		/*
		* MemAllocator is a memory allocator, whose memory is preallocate at the time when the object is created.
		*
		* [T] is the class type.
		* [size] is the max size of the allocator.
		* [is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename T, bool _is_debug>
		class GadtAllocator
		{
		private:
			using pointer = T*;
			using reference = T&;
			using index_queue = std::queue<size_t>;

			static const size_t		_size = sizeof(T);
			const size_t			_count;
			index_queue				_available_index;
			pointer					_fir_element;
			bool*					_exist_element;

		private:
			//get index by pointer.
			inline size_t ptr_to_index(pointer p) const
			{
				uintptr_t t = uintptr_t(p);
				uintptr_t fir = uintptr_t(_fir_element);
				return size_t((t - fir) / _size);
			}

			//get objecy prt by index. return nullptr if the index is overflow.
			inline pointer index_to_ptr(size_t index) const
			{
				return _fir_element + index;
			}

			//destory element by index.
			inline void destory_by_index(size_t index)
			{
				index_to_ptr(index)->~T();
				_available_index.push(index);
				_exist_element[index] = false;
			}

			//clear available pointer queue
			inline void clear_queue()
			{
				_available_index = index_queue();
			}

			//allocate memory
			inline void alloc_memory(size_t count)
			{
				//_fir_element = reinterpret_cast<T*>(new char[count * _size]);
				_fir_element = reinterpret_cast<T*>(calloc(count, _size));
				_exist_element = new bool[count];
			}

			//delete memory
			inline void delete_memory()
			{
				delete[] _exist_element;
				//delete[] _fir_element;
				::free(_fir_element);
				_exist_element = nullptr;
				_fir_element = nullptr;
			}

			//allocate memery by size.
			void allocate(size_t count)
			{
				alloc_memory(count);
				for (size_t i = 0; i < count; i++)
				{
					_available_index.push(i);
					_exist_element[i] = false;
				}
			}

			//deallocate memory.
			void deallocate()
			{
				for (size_t i = 0; i < _count; i++)
				{
					if (_exist_element[i])
					{
						destory_by_index(i);
					}
				}
				clear_queue();
				delete_memory();
			}

		public:
			//constructor function with allocation.
			GadtAllocator(size_t count) :
				_count(count),
				_available_index(),
				_fir_element(nullptr),
				_exist_element(nullptr)
			{
				allocate(count);
			}

			//copy constructor function.
			GadtAllocator(const GadtAllocator& target) :
				_count(target._count),
				_available_index(target._available_index),
				_fir_element(nullptr),
				_exist_element(nullptr)
			{
				alloc_memory(_count);
				for (size_t i = 0; i < _count; i++)
				{
					this->_exist_element[i] = target._exist_element[i];
					if (_exist_element[i])
					{
						*(index_to_ptr(i)) = *(target.index_to_ptr(i));
					}
				}
			}

			//destructor function.
			~GadtAllocator()
			{
				deallocate();
			}

			//free space by ptr, return true if free successfully.
			inline bool destory(pointer target)
			{
				uintptr_t t = uintptr_t(target);
				uintptr_t fir = uintptr_t(_fir_element);
				uintptr_t last = uintptr_t(_fir_element + _count);
				if (target != nullptr && t >= fir && t < last && ((t - fir) % _size == 0))
				{
					size_t index = (t - fir) / sizeof(T);
					if (_exist_element[index])
					{
						destory_by_index(index);
						return true;
					}
				}
				return false;
			}

			//copy source object to a empty space and return the pointer, return nullptr if there are not available space.
			template<class... Types>
			pointer construct(Types&&... args)//T* constructor(const T& source)
			{
				if (_available_index.empty() != true)
				{
					size_t index = _available_index.front();
					_exist_element[index] = true;
					_available_index.pop();
					pointer ptr = index_to_ptr(index);
					ptr = new (ptr) T(std::forward<Types>(args)...);//placement new;
					return ptr;
				}
				return nullptr;
			}

			//total size of alloc.
			inline size_t total_size() const
			{
				return _count;
			}

			//remain size in the alloc.
			inline size_t remain_size() const
			{
				return _available_index.size();
			}

			//return true if there is not available space in this allocator.
			inline bool is_full() const
			{
				return _available_index.size() == 0;
			}

			//flush all datas in the allocator.
			inline void flush()
			{
				clear_queue();
				for (size_t i = 0; i <_count; i++)
				{
					if (_exist_element[i] == true)
					{
						destory_by_index(i);
					}
					_available_index.push(_elements + i);
				}
			}

			//get info as string format
			inline std::string info() const
			{
				std::stringstream ss;
				ss << "{count : " << _count << ", remain: " << remain_size() << "}";
				return ss.str();
			}

			//return the value of _is_debug.
			constexpr inline bool is_debug() const
			{
				return _is_debug;
			}
		};

		template<typename T, bool _is_debug>
		class GadtListNode
		{
		public:
			using pointer = GadtListNode<T, _is_debug>*;

		private:
			const T _value;
			pointer _next_node;

		public:
			//constructor function.
			inline GadtListNode(const T& value) :
				_value(value),
				_next_node(nullptr)
			{
			}

			//copy constructor function is disallowed.
			GadtListNode(const GadtListNode&) = delete;

			inline const T& value() const { return _value; }
			inline pointer next_node() const { return _next_node; }
			inline void set_next_node(pointer p) { _next_node = p; }

		};

		template<typename T, bool _is_debug>
		class GadtList
		{
		public:
			using ListNode = GadtListNode<T, _is_debug>;
			using Allocator = GadtAllocator<ListNode, _is_debug>;
			using node_pointer = GadtListNode<T, _is_debug>*;

		private:
			const bool   _private_allocator;
			Allocator&	 _allocator;
			node_pointer _first_node;
			node_pointer _last_node;
			node_pointer _iterator;

		public:
			GadtList(size_t allocator_count) :
				_private_allocator(true),
				_allocator(*(new Allocator(allocator_count))),
				_first_node(nullptr),
				_last_node(nullptr),
				_iterator(nullptr)
			{
			}

			GadtList(Allocator& allocator) :
				_private_allocator(false),
				_allocator(allocator),
				_first_node(nullptr),
				_last_node(nullptr),
				_iterator(nullptr)
			{
			}

			inline ~GadtList()
			{
				if (_private_allocator)
				{
					delete &_allocator;
				}
			}

			//insert a new value in the end of the list.
			void insert(const T& T)
			{
				auto ptr = _allocator.construct(T);
				if (_first_node == nullptr)
				{
					_first_node = ptr;
					_last_node = ptr;
					_iterator = ptr;
				}
				else
				{
					_last_node->set_next_node(ptr);
					_last_node = _last_node->next_node();
				}
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
	}
}