#include "gadtlib.h"
#include "visual_tree.h"

#pragma once

namespace gadt
{
	namespace log
	{
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
			std::string		_json_output_path;
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
				_json_output_path(),
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
				_json_output_path(),
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
			inline std::string json_output_path() const
			{
				return _json_output_path;
			}

			//get ref of visual tree.
			inline VisualTree& visual_tree()
			{
				return _visual_tree;
			}

			//get state to string function.
			inline StateToStrFunc state_to_str_func()
			{
				return _state_to_str_func;
			}

			//get action to string function.
			inline ActionToStrFunc action_to_str_func()
			{
				return _action_to_str_func;
			}

			//get result to string function.
			inline ResultToStrFunc result_to_str_func()
			{
				return _result_to_str_func;
			}

			//initialize logger.
			void Init(
				StateToStrFunc state_to_str_func,
				ActionToStrFunc action_to_str_func,
				ResultToStrFunc result_to_str_func = [](const Result&)->std::string {return ""; }
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
			inline void EnableJsonOutput(std::string json_output_path)
			{
				_enable_json_output = true;
				_json_output_path = json_output_path;
			}

			//disable json output.
			inline void DisableJsonOutput()
			{
				_enable_json_output = false;
			}

			//output json to path.
			inline void OutputJson() const
			{
				std::ofstream ofs(_json_output_path);
				_visual_tree.output_json(ofs);
			}

		};
	}

	namespace stl
	{
		/*
		* Allocator is a memory allocator, whose memory is preallocate at the time when the object is created.
		*
		* [T] is the class type.
		* [size] is the max size of the allocator.
		* [is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename T, bool _is_debug = false>
		class Allocator
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
			Allocator(size_t count) :
				_count(count),
				_available_index(),
				_fir_element(nullptr),
				_exist_element(nullptr)
			{
				allocate(count);
			}

			//copy constructor function.
			Allocator(const Allocator& target) :
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
			~Allocator()
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
		* ListNode is the basic unit of gadt::stl::List.
		*
		* [T] is the class type of the link list.
		* [is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename T, bool _is_debug = false>
		class ListNode
		{
		public:
			using pointer = ListNode<T, _is_debug>*;

		private:
			const T _value;
			pointer _next_node;

		public:
			//constructor function.
			inline ListNode(const T& value) :
				_value(value),
				_next_node(nullptr)
			{
			}

			//copy constructor function is disallowed.
			ListNode(const ListNode&) = delete;

			inline const T& value() const { return _value; }
			inline pointer next_node() const { return _next_node; }
			inline void set_next_node(pointer p) { _next_node = p; }
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
			using Node = ListNode<T, _is_debug>;
			using Allocator = gadt::stl::Allocator<Node, _is_debug>;
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
			void insert(const T& value)
			{
				_size++;//incr size of the list.
				auto ptr = _allocator.construct(value);
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

		template<typename T, bool _is_debug = false>
		class LinearAllocator
		{
		private:
			using pointer = T*;
			using reference = T&;

			static const size_t		_size = sizeof(T);
			const size_t			_count;
			pointer					_fir_element;
			size_t					_length;

		private:
			//allocate memory
			inline void alloc_memory(size_t count)
			{
				_fir_element = reinterpret_cast<T*>(calloc(count, _size));
			}

			//delete memory
			inline void delete_memory()
			{
				//delete[] _fir_element;
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
				for (size_t i = 0; i < _length; i++)
				{
					construct_next(*(target._fir_element + i));
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
			pointer construct_next(Types&&... args)//T* constructor(const T& source)
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
				GADT_CHECK_WARNING(is_debug(), index >= _length, "out of range");
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

	namespace random
	{
		

		template<typename T, bool _is_debug = false>
		class RandomPool
		{
		public:
			template<typename T>
			struct RandomPoolElement
			{
				const size_t weight;
				const size_t lower_limit;
				T data;

				template<class... Types>
				RandomPoolElement(size_t _weight, size_t _lower_limit, Types&&... args) :
					weight(_weight),
					lower_limit(_lower_limit),
					data(std::forward<Types>(args)...)
				{
				}
			};

		private:
			using pointer = T*;
			using reference = T&;
			using Element = RandomPoolElement<T>;
			using Allocator = stl::LinearAllocator<Element, _is_debug>;

		private:

			Allocator	_ele_alloc;
			size_t		_accumulated_range;

		public:
			//default constructor.
			RandomPool(size_t max_size):
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
				if (_ele_alloc.construct_next(weight, _accumulated_range, data))
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
				if(_ele_alloc.construct_next(weight, _accumulated_range, args));
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
			inline const pointer get_element(size_t index) const
			{
				return _ele_alloc[index];
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
				if (size() > 0)
				{
					size_t rnd = rand() % _accumulated_range;
					for (size_t i = 0; i < size(); i++)
					{
						if (_ele_alloc[i]->lower_limit >= rnd)
						{
							return *_ele_alloc[i];
						}
					}
					GADT_CHECK_WARNING(_is_debug, true, "unsuccessful random pick up.");
				}
				return nullptr;
			}

			//get the size of the element.
			inline size_t size() const
			{
				return _ele_alloc.size();
			}

			const T& operator[](size_t index)
			{
				return get_element(index);
			}
		};
	}
}