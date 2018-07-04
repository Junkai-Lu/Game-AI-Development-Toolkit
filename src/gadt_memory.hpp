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
#include "visual_tree.h"

#pragma once

#ifdef GADT_WARNING
	#define GADT_ALLOCATOR_DEBUG_INFO
#endif

namespace gadt
{
	namespace stl
	{
		/*
		* StackAllocator is a memory allocator, whose memory is preallocate at the time when the object is created.
		* its memory is allocated by stack, all the element in it can be opearted.
		*
		* [T] is the class type.
		* [size] is the max size of the allocator.
		* [is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename T, bool _is_debug = false>
		class StackAllocator final
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

			//return the value of _is_debug.
			constexpr inline bool is_debug() const
			{
				return _is_debug;
			}

		public:
			//constructor function with allocation.
			StackAllocator(size_t count) :
				_count(count),
				_available_index(),
				_fir_element(nullptr),
				_exist_element(nullptr)
			{
				allocate(count);
			}

			//copy constructor function.
			StackAllocator(const StackAllocator& target) :
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
			~StackAllocator()
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

			//return size of the allocator.
			inline size_t size() const
			{
				return _count - _available_index.size();
			}

			//return true if there is not available space in this allocator.
			inline bool is_full() const
			{
				return _available_index.size() == 0;
			}

			//return true if this allocator is empty.
			inline bool is_empty() const
			{
				return _available_index.size() == _count;
			}

			//flush all datas in the allocator.
			inline void flush()
			{
				clear_queue();
				for (size_t i = 0; i < _count; i++)
				{
					if (_exist_element[i] == true)
					{
						destory_by_index(i);
					}
					//_available_index.push(ptr_to_index(_fir_element + i));
				}
			}

			//get info as string format
			inline std::string info() const
			{
				std::stringstream ss;
				ss << "{count : " << _count << ", remain: " << remain_size() << "}";
				return ss.str();
			}

			
		};

		/*
		* LinearAllocator is a memory allocator, whose memory is preallocate at the time when the object is created.
		* its memory is allocated by array, can only the last element could be operated.
		*
		* [T] is the class type.
		* [size] is the max size of the allocator.
		* [is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename T, bool _is_debug = false>
		class LinearAllocator final
		{
		private:
			using pointer = T*;
			using reference = T&;

			static const size_t		_size = sizeof(T);
			const size_t			_count;
			pointer					_fir_element;
			size_t					_length;

#ifdef GADT_ALLOCATOR_DEBUG_INFO
			std::vector<pointer>	_pointers;
#endif

		private:
			//allocate memory
			inline void alloc_memory(size_t count)
			{
				_fir_element = reinterpret_cast<T*>(calloc(count, _size));
#ifdef GADT_ALLOCATOR_DEBUG_INFO
				for (size_t i = 0; i < count; i++)
					_pointers.push_back(_fir_element + i);
#endif
			}

			//delete memory
			inline void delete_memory()
			{
				while (destory_last() == true) {}
				::free(_fir_element);
				_fir_element = nullptr;
				_length = 0;
			}

			//return the value of _is_debug.
			constexpr inline bool is_debug() const
			{
				return _is_debug;
			}

		public:
			//constructor function with allocation.
			LinearAllocator(size_t count) :
				_count(count),
				_fir_element(nullptr),
				_length(0)
			{
				alloc_memory(count);
			}

			//copy constructor function.
			LinearAllocator(const LinearAllocator& target) :
				_count(target._count),
				_fir_element(nullptr),
				_length(0)
			{
				alloc_memory(_count);
				for (size_t i = 0; i < target._length; i++)
				{
					construct(*(target._fir_element + i));
				}
			}

			//destructor function.
			~LinearAllocator()
			{
				delete_memory();
			}

			//free space by ptr, return true if free successfully.
			inline bool destory_last()
			{
				if (_length > 0)
				{
					pointer last = _fir_element + (_length - 1);
					_length--;
					last->~T();
					return true;
				}
				return false;
			}

			//copy source object to a empty space and return the pointer, return nullptr if there are not available space.
			template<class... Types>
			pointer construct(Types&&... args)//T* constructor(const T& source)
			{
				if (is_full() == false)
				{
					pointer ptr = _fir_element + _length;
					_length++;
					ptr = new (ptr) T(std::forward<Types>(args)...);//placement new;
					return ptr;
				}
				return nullptr;
			}

			//get first element.
			pointer element(size_t index) const
			{
				GADT_WARNING_IF(is_debug(), index >= _length, "out of range");
				return _fir_element + index;
			}

			//total size of alloc.
			inline size_t total_size() const
			{
				return _count;
			}

			//remain size in the alloc.
			inline size_t remain_size() const
			{
				return _count - _length;
			}

			//return size of the allocator.
			inline size_t size() const
			{
				return _length;
			}

			//return true if there is not available space in this allocator.
			inline bool is_full() const
			{
				return _count == _length;
			}

			//return true if this allocator is empty.
			inline bool is_empty() const
			{
				return _length == 0;
			}

			//flush all datas in the allocator.
			inline void flush()
			{
				while (destory_last()) {}
			}

			//get info as string format
			inline std::string info() const
			{
				std::stringstream ss;
				ss << "{count : " << _count << ", remain: " << remain_size() << "}";
				return ss.str();
			}

			pointer operator[](size_t index) const
			{
				return element(index);
			}
		};		
	}	
}