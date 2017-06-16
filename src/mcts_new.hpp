/*
* General monte carlo tree search algorithm lib.
*
* a search framework for perfect information sequential games, which allow
* user to insert extra function or redefine default functions to custom the
* search algorithm.
*/

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

#define ENABLE_TREE_VISUALIZATION	//allow convert search tree to xml file for tree visualization.

#include "gadtlib.h"

#pragma once

namespace gadt
{
	namespace mcts_new
	{
		//AgentIndex is the type index of each player, default is int8_t.
		using AgentIndex = int8_t;
		using UcbValueType = double;

		/*
		* MemAllocator is a memory allocator, whose memory is preallocate at the time when the object is created.
		*
		* [T] is the class type.
		* [size] is the max size of the allocator.
		* [is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename T, bool is_debug>
		class MctsAllocator
		{
		private:
			using pointer = T*;
			using reference = T&;
			
			const size_t			_max_size;
			std::queue<T*>			_available_ptr;
			pointer					_elements;


		private:
			//get index by pointer.
			inline size_t ptr_to_index(pointer p) const
			{
				uintptr_t t = uintptr_t(p);
				uintptr_t fir = uintptr_t(_elements);
				return size_t((t - fir) / sizeof(T));
			}

			//free space by index.
			inline bool free_by_index(size_t index)
			{
				if (index < _max_size)
				{
					_available_ptr.push(_elements + index);
					return true;
				}
				return false;
			}

			//get objecy prt by index. return nullptr if the index is overflow.
			pointer get_by_index(size_t index) const
			{
				if (index < _max_size)
				{
					return _elements + index;
				}
				return nullptr;
			}

		public:
			//default constructor function.
			MctsAllocator(size_t max_size):
				_max_size(max_size)
			{
				_elements = reinterpret_cast<T*>(new char[_max_size * sizeof(T)]);
				flush();
			}

			//copy constructor function.
			MctsAllocator(const MctsAllocator& target):
				_max_size(target._max_size)
			{
				_elements = reinterpret_cast<T*>(new char[_max_size * sizeof(T)]);
				std::vector<bool> is_available(_max_size, false);
				std::queue<pointer> temp_ptrs = target._available_ptr;
				while (!temp_ptrs.empty())
				{
					is_available[target.ptr_to_index(temp_ptrs.front())] = true;
					temp_ptrs.pop();
				}

				for (size_t i = 0; i < _max_size; i++)
				{
					if (is_available[i])
					{
						this->free_by_index(i);
					}
					else
					{
						(*get_by_index(i)) = (*target.get_by_index(i));
					}
				}
			}

			//destructor function.
			~MctsAllocator()
			{
				delete[] _elements;
			}

			//free space by ptr, return true if free successfully.
			inline bool free(pointer target)
			{
				uintptr_t t = uintptr_t(target);
				uintptr_t fir = uintptr_t(_elements);
				uintptr_t last = uintptr_t(_elements + _max_size);
				if (target != nullptr && t >= fir && t < last && ((t - fir) % sizeof(T) == 0))
				{
					_available_ptr.push(target);
					return true;
				}
				return false;
			}

			//copy source object to a empty space and return the pointer, return nullptr if there are not available space.
			template<class... Types>
			pointer construct(Types&&... args)//T* constructor(const T& source)
			{
				if (_available_ptr.empty() != true)
				{
					pointer temp = _available_ptr.front();
					*temp = T(std::forward<Types>(args)...);
					_available_ptr.pop();
					return temp;
				}
				return nullptr;
			}

			//total size of alloc.
			inline size_t total_size() const
			{
				return _max_size;
			}

			//remain size in the alloc.
			inline size_t remain_size() const
			{
				return _available_ptr.size();
			}

			//return true if there is not available space in this allocator.
			inline bool is_full() const
			{
				return _available_ptr.size() == 0;
			}

			//flush all datas in the allocator.
			inline void flush()
			{
				for (size_t i = 0; i <_max_size; i++)
				{
					_available_ptr.push(_elements + i);
				}
			}
		};

		/*
		* MctsNode is the node class in the monte carlo tree search.
		*
		* [State] is the game-state class, which is defined by the user.
		* [Action] is the game-action class, which is defined by the user.
		* [Result] is the game-result class, which stand for a terminal state of the game.
		* [is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename State, typename Action, typename Result, bool is_debug>
		class MctsNode
		{
		public:											
			using ActionSet			= std::vector<Action>;								//ActionSet is the set of Action.
			using NodePtrSet		= std::vector<MctsNode*>;							//ChildSet is the set of ptrs to child nodes.
			using MakeActionFunc	= std::function<void(const State&, ActionSet&)>;	//the function which create action set by the state.
			using CheckResultFunc	= std::function<int(const State&)>;					//the function that check the simulation result in the backpropagation stage.
			using FreeSelfFunc		= std::function<void(MctsNode*)>;					//free self from allocator.

		private:
			State			_state;				//state of this node.
			AgentIndex		_owner_index;		//the belonging player index of this index.
			AgentIndex		_winner_index;		//the winner index of the state.
			uint32_t		_visited_time;		//how many times that this node had been visited.
			uint32_t		_win_time;			//win time accmulated by the simulation.
			uint8_t			_next_action_index;	//the index of next action.
			ActionSet		_action_set;		//action set of this node.
			NodePtrSet		_child_nodes;		//the ptr of child nodes.

			const MakeActionFunc	_make_action;	//make action function would generate actions from current state.		
			const CheckResultFunc	_check_result;	//check result function would return the winner of current state. 

		public:
			const State&		state()					const { return _state; }
			const AgentIndex	owner_index()			const { return _owner_index; }
			const AgentIndex	winner_index()			const { return _winner_index; }
			const uint32_t		visited_time()			const { return _visited_time; }
			const uint32_t		win_time()				const { return _win_time; }
			const uint8_t		next_action_index()		const { return _next_action_index; }
			const Action&		action(size_t i)		const { return _action_set[i]; }
			const MctsNode*		child_node(size_t i)	const { return _child_nodes[i]; }
			const size_t		child_num()				const { return _child_nodes.size(); }
			const NodePtrSet&	child_set()				const { return _child_nodes; }
			const ActionSet&	action_set()			const { return _action_set; }

		private:
			//a value means no winner, which is differ from any other AgentIndex.
			static const AgentIndex _no_winner_index = 0;

			//exist unactived action in the action set.
			inline bool exist_unactivated_action() const
			{
				return _next_action_index < _action_set.size();
			}

			//get next action.
			inline const Action& next_action()
			{
				return _action_set[_next_action_index++];
			}

			//return true if the state is the terminal-state of the game.
			inline bool is_end_state() const
			{
				return _winner_index != _no_winner_index;
			}

		public:
			MctsNode(const State& state, AgentIndex owner_index) :
				_state(state),
				_owner_index(owner_index),
				_visited_time(0),
				_win_time(0),
				_next_action_index(0)
			{
				_make_action(_state, _action_set);
				_child_nodes.resize(_action_set.size(), nullptr);
			}

			//free the node from allocator.
			void FreeFromAllocator(MctsAllocator<MctsNode,is_debug>& allocator)
			{
				//free all child node if possible.
				for (auto p : _child_nodes)
				{
					if (p != nullptr)
					{
						p->FreeFromAllocator(allocator);
					}
				}
				
				//free the node itself.
				if (is_debug)
				{
					bool b = allocator.free(this);
					GADT_WARNING_CHECK(b == false, "free child node failed.");
				}
				else
				{
					allocator.free(this);
				}
			}
		};

		/*
		* MctsSearch is a template of monte carlo tree search.
		*
		* [State] is the game-state class, which is defined by the user.
		* [Action] is the game-action class, which is defined by the user.
		* [Result] is the game-result class, which stand for a terminal state of the game.
		* [is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename State, typename Action, typename Result, bool is_debug = false>
		class MctsSearch
		{
		public:
			using Node = MctsNode<State, Action, Result, is_debug>;							
			using Allocator = MctsAllocator<Node, is_debug>;					

			using ActionSet				= std::vector<Action>;									//ActionSet is the set of Action
			using NodePtrSet			= std::vector<Node*>;								//ChildSet is the set of ptrs to child nodes.
			using GetNewStateFunc		= std::function<State(const State&, const Action&)>;	//get a new state from previous state and action.
			using MakeActionFunc		= std::function<void(const State&, ActionSet&)>;		//the function which create action set by the state.
			using DetemineWinnerFunc	= std::function<AgentIndex(const Node&)>;			//detemine the winner of the giving node.
			using CheckResultFunc		= std::function<AgentIndex(const State&)>;				//deal with the the simulation result in the backpropagation stage.
			using BackPropagateFunc		= std::function<void(const Result&, Node&)>;		//modify values in the node by the result.
			using UcbForParentFunc		= std::function<UcbValueType(const Node&)>;			//return ucb value for parent node.
			using DefaultPolicyFunc		= std::function<Result(const Node&)>;				//get a result from node by excute default policy.
			using AllowExtendFunc		= std::function<bool(const Node&)>;					//allow node to extend child node.
			
		public:
			const GetNewStateFunc		GetNewState;
			const MakeActionFunc		MakeAction;		
			const DetemineWinnerFunc	DetemineWinner;
			CheckResultFunc				CheckResult;
			BackPropagateFunc			BackPropagate;
			UcbForParentFunc			UcbForParent;
			DefaultPolicyFunc			DefaultPolicy;
			AllowExtendFunc				AllowExtend;

		private:
			Allocator&		_allocator;				//the allocator for the search.
			const bool		_private_allocator;		//use private allocator.
			double			_timeout;				//set timeout (seconds).
			size_t			_max_iteration;			//set max iteration times.
			bool			_enable_gc;				//allow garbage collection if the tree run out of memory.
			
		private:
			//define functions as default.
			void FuncInit()
			{
			}

			//excute iteration function.
			Action ExcuteMCTS(State root_state, AgentIndex owner_index, double timeout, size_t max_iteration, bool enable_gc)
			{
				Node root_node(root_state, owner_index);
				timer::TimePoint start_tp;
				size_t iteration_time = 0;
				for (iteration_time = 0; iteration_time < max_iteration; iteration_time++)
				{
					if (start_tp.time_since_created() > timeout)
					{
						break;//timeout, stop search.
					}
					if (_allocator.is_full())
					{
						if (enable_gc)
						{
							//do garbage collection.
						}
						else
						{
							break;//run out of memory, stop search.
						}
					}
					
					//do iteration to extend search tree.

				}

				//log info if is debuging.


				//return the best result
			}

		public:
			//use private allocator.
			MctsSearch(
				MakeActionFunc _MakeAction, 
				CheckResultFunc _CheckResult, 
				DetemineWinnerFunc _DetemineWinner,
				size_t max_node
			):
				_allocator(*(new Allocator(max_node))),
				_private_allocator(true),
				MakeAction(_MakeAction),
				CheckResult(_CheckResult),
				DetemineWinner(_DetemineWinner)
			{
				FuncInit();
			}

			//use public allocator.
			MctsSearch(
				MakeActionFunc _MakeAction,
				CheckResultFunc _CheckResult,
				DetemineWinnerFunc _DetemineWinner,
				Allocator allocator
			):
				_allocator(allocator),
				_private_allocator(false),
				MakeAction(_MakeAction),
				CheckResult(_CheckResult),
				DetemineWinner(_DetemineWinner)
			{
			}

			//deconstructor function
			~MctsSearch()
			{
				if (_private_allocator)
				{
					delete &_allocator;
				}
			}

			//do search with default parameters.
			Action DoMcts(const State root_state, AgentIndex owner_index)
			{
				return ExcuteMCTS(root_state, owner_index, _timeout, _max_iteration, _enable_gc);
			}

			//do search with custom parameters.
			Action DoMcts(const State root_state, AgentIndex owner_index, double timeout, size_t max_iteration, bool enable_gc)
			{
				return ExcuteMCTS(root_state, owner_index, timeout, max_iteration, enable_gc);
			}
		};

	}
}