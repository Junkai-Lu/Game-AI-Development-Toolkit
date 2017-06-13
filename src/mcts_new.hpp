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

		//MctsResult is the simulation result in the mcts. it include the index of winner and the final state of the game;
		template<typename State, bool enable_debug_model = false>
		class MctsResult
		{
		private:
			const State	_state;			//the final state of the simulation.
			const AgentIndex _winner;	//winner in the result.

		public:
			inline MctsResult(const State& state, int winner):
				_state(state),
				_winner(winner)
			{
			}

			const State& state() const
			{
				return _state;
			}

			int winner() const
			{
				return _winner;
			}

		};

		/*
		* MctsNode is the node class in the monte carlo tree search.
		* [state_type] is the game-state class, which is defined by the user.
		* [action_type] is the game-action class, which is defined by the user.
		* [enable_debug_model] means enable the debug model, if it is enabled, some debug info would not be ignored. this may result in the degradation of performance.
		*/
		template<typename State, typename Action, typename Result, bool enable_debug_model = false>
		class MctsNode
		{
		public:											
			using ActionSet = std::vector<Action>;									//ActionSet is the set of Action, each action would lead to a new state.
			using NodePtrSet = std::vector<MctsNode*>;								//ChildSet is the set of ptrs to child nodes.
			using MakeActionFunc = std::function<void(const State&, ActionSet&)>;	//the function which create action set by the state.
			using CheckResultFunc = std::function<int(const State&)>;				//the function that check the simulation result in the backpropagation stage.

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


		};

		/*
		* MemAllocator is a memory allocator, whose memory is preallocate at the time when the object is created.
		* [T] is the class type.
		*/
		template<typename T, size_t size>
		class MctsAllocator
		{
		private:
			std::allocator<T>	alloc;
			std::stack<T*>		available_ptr;

		public:
			MemAllocator()
			{
				auto p = alloc.allocate(size);
				for (size_t i = 0; i < size; i++)
				{
					_available_space.push(p + i);
				}
			}

			//to free a space by index.
			void free(size_t index)
			{
				GADT_WARNING_CHECK(index < size, "index overflow");
				_available_space.push(&_memory[index]);
			}

			//to free a space by ptr.
			void free(T* target)
			{
				_available_space.push(target);
			}

			//copy source object to a empty space and return the pointer, return nullptr if there are not available space.
			T* get(const T& source)
			{
				if (_available_space.empty() != true)
				{
					T* temp = _available_space.top();
					_available_space.pop();
					*temp = source;
					return temp;
				}
				return nullptr;
			}

			//
			const T& get_object(size_t index) const
			{
				GADT_WARNING_CHECK(index >= size, "index overflow");
				return _memory[index];
			}

			size_t total_size() const
			{
				return size;
			}

			size_t available_size() const
			{
				return _available_space.size();
			}
		};

		template<typename State, typename Action, typename Result, bool enable_debug_model = false>
		class McteSearch
		{
			
		};

	}
}