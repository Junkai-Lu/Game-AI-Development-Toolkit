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
			std::string		_json_output_folder;
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
				_json_output_folder(),
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
			inline std::string json_output_folder() const
			{
				return _json_output_folder;
			}

			//get ref of visual tree.
			inline VisualTree& visual_tree()
			{
				return _visual_tree;
			}

			//get state to string function.
			inline StateToStrFunc state_to_str_func() const
			{
				return _state_to_str_func;
			}

			//get action to string function.
			inline ActionToStrFunc action_to_str_func() const
			{
				return _action_to_str_func;
			}

			//get result to string function.
			inline ResultToStrFunc result_to_str_func() const
			{
				return _result_to_str_func;
			}

			//initialize logger.
			void Init(
				StateToStrFunc state_to_str_func,
				ActionToStrFunc action_to_str_func,
				ResultToStrFunc result_to_str_func
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
			inline void EnableJsonOutput(std::string json_output_folder)
			{
				_enable_json_output = true;
				_json_output_folder = json_output_folder;
			}

			//disable json output.
			inline void DisableJsonOutput()
			{
				_enable_json_output = false;
			}

			//output json to path.
			inline void OutputJson() const
			{
				std::string path = "./" + _json_output_folder;
				if (!file::DirExist(path))
				{
					file::MakeDir(path);
				}
				path += "/";
				path += timer::TimeString();
				path += ".json";
				std::ofstream ofs(path);
				_visual_tree.output_json(ofs);
			}

		};
	}

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

		private:
			//allocate memory
			inline void alloc_memory(size_t count)
			{
				_fir_element = reinterpret_cast<T*>(calloc(count, _size));
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
				for (size_t i = 0; i < _length; i++)
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

		template<typename Action, bool _is_debug>
		class ActionPool
		{
		private:
			using ActionList = List<Action, _is_debug>;
			using pointer = typename ActionList::pointer;
			using Allocator = typename ActionList::Allocator;

		private:
			ActionList _action_list;

		public:
			
			inline ActionPool(Allocator& allocator):
				_action_list(allocator)
			{
			}

			inline void push_back(Action action)
			{
				_action_list.push_back(action);
			}

			inline void size() const
			{
				return _action_list.size();
			}

			inline pointer next_action() const
			{
				pointer next = _action_list.iterator();
				_action_list.to_next_iterator();
				return next;
			}

			inline bool is_end() const
			{
				return _action_list.is_end();
			}
		};
	}

	namespace random
	{
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
				table::ConsoleTable tb(3, _ele_alloc.size()+1);
				tb.set_cell_in_row(0, { {"index"},{"weight"},{"range"} });
				tb.set_width({ 6,6,10 });
				tb.enable_title({ "random pool" });
				for (size_t i = 0; i < _ele_alloc.size(); i++)
				{
					tb.set_cell_in_row(i + 1, { 
						{console::ToString(i)},
						{console::ToString(get_weight(i))},
						{_ele_alloc.element(i)->range()}
					});
				}
				return tb.output_string();
			}

			const reference operator[](size_t index)
			{
				return get_element(index);
			}

			
		};
	}

	namespace policy
	{
		inline UcbValue UCB1(UcbValue average_reward, UcbValue overall_time, UcbValue played_time, UcbValue c = 1.41421)
		{
			UcbValue ln = log10(overall_time);
			UcbValue exploration = sqrt(ln / played_time);
			return average_reward + c * exploration;
		}
	}
}