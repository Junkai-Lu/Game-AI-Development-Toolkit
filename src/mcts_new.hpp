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
		using AgentIndex		= int8_t;
		using UcbValue			= double;

		namespace policy
		{
			inline UcbValue UCB1(UcbValue average_reward, UcbValue overall_time, UcbValue played_time, UcbValue c = 1)
			{
				return average_reward + c * static_cast<UcbValue>(sqrt(2 * log10(overall_time) / played_time));
			}
		}

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
			using Node			= typename MctsNode<State, Action, Result, is_debug>;		//MctsNode
			using Allocator		= typename MctsAllocator<Node, is_debug>;					//MctsAllocate 
			using ActionSet		= typename std::vector<Action>;								//ActionSet is the set of Action.
			using NodePtrSet	= typename std::vector<MctsNode*>;							//ChildSet is the set of ptrs to child nodes.

			//function package
			struct FuncPackage
			{
			public:
				using GetNewStateFunc		= std::function<State(const State&, const Action&)>;	//get a new state from previous state and action.
				using MakeActionFunc		= std::function<void(const State&, ActionSet&)>;		//the function which create action set by the state.
				using DetemineWinnerFunc	= std::function<AgentIndex(const State&)>;				//return no_winner_index if a state is not terminal state.
				using StateToResultFunc		= std::function<Result(const State&, AgentIndex)>;		//get a result from state and winner.
				using AllowUpdateValueFunc	= std::function<bool(const Result&, const State&)>;		//update values in the node by the result.
				using TreePolicyValueFunc	= std::function<UcbValue(const Node&, const Node&)>;	//value of child node in selection process. the highest would be seleced.
				using DefaultPolicyFunc		= std::function<const Action&(const ActionSet&)>;		//the default policy to select action.
				using AllowExtendFunc		= std::function<bool(const Node&)>;						//allow node to extend child node.
				using AllowExcuteGcFunc		= std::function<bool(const Node&)>;						//the condition to excute gc in a node.
				using ValueForRootNodeFunc	= std::function<UcbValue(const Node&)>;					//select best action of root node after iterations finished.

			public:
				//necessary functions.
				const GetNewStateFunc		GetNewState;
				const MakeActionFunc		MakeAction;
				const DetemineWinnerFunc	DetemineWinner;
				const StateToResultFunc		StateToResult;
				const AllowUpdateValueFunc	AllowUpdateValue;

				//default functions.
				TreePolicyValueFunc			TreePolicyValue;
				DefaultPolicyFunc			DefaultPolicy;
				AllowExtendFunc				AllowExtend;
				AllowExcuteGcFunc			AllowExcuteGc;
				ValueForRootNodeFunc		ValueForRootNode;

			public:
				FuncPackage(
					GetNewStateFunc			_GetNewState,
					MakeActionFunc			_MakeAction,
					DetemineWinnerFunc		_DetemineWinner,
					StateToResultFunc		_StateToResult,
					AllowUpdateValueFunc	_AllowUpdateValue,
					TreePolicyValueFunc		_TreePolicyValue,
					DefaultPolicyFunc		_DefaultPolicy,
					AllowExtendFunc			_AllowExtend,
					AllowExcuteGcFunc		_AllowExcuteGc,
					ValueForRootNodeFunc		_ValueForRootNode
				):
					GetNewState			(_GetNewState),
					MakeAction			(_MakeAction),
					DetemineWinner		(_DetemineWinner),
					StateToResult		(_StateToResult),
					AllowUpdateValue	(_AllowUpdateValue),
					TreePolicyValue		(_TreePolicyValue),
					DefaultPolicy		(_DefaultPolicy),
					AllowExtend			(_AllowExtend),
					AllowExcuteGc		(_AllowExcuteGc),
					ValueForRootNode	(_ValueForRootNode)
				{
				}

				FuncPackage(
					GetNewStateFunc			_GetNewState,
					MakeActionFunc			_MakeAction,
					DetemineWinnerFunc		_DetemineWinner,
					StateToResultFunc		_StateToResult,
					AllowUpdateValueFunc	_AllowUpdateValue
				):
					GetNewState			(_GetNewState),
					MakeAction			(_MakeAction),
					DetemineWinner		(_DetemineWinner),
					StateToResult		(_StateToResult),
					AllowUpdateValue	(_AllowUpdateValue)
				{
				}
			};

		private:
			State			_state;				//state of this node.
			AgentIndex		_winner_index;		//the winner index of the state.
			uint32_t		_visited_time;		//how many times that this node had been visited.
			uint32_t		_win_time;			//win time accmulated by the simulation.
			uint8_t			_next_action_index;	//the index of next action.
			ActionSet		_action_set;		//action set of this node.
			NodePtrSet		_child_nodes;		//the ptr of child nodes.

		public:
			const State&		state()					const { return _state; }
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
			static const size_t		_default_policy_warning_length = 1000;

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

			//set the ptr of next child.
			inline void set_next_child(Node* ptr)
			{
				_child_nodes[_next_action_index] = ptr;
			}

			//move the cursor to next action.
			inline void to_next_action()
			{
				_next_action_index++;
			}

			//return true if the state is the terminal-state of the game.
			inline bool is_end_state() const
			{
				return _winner_index != _no_winner_index;
			}

			//increase visited time.
			inline void incr_visited_time()
			{
				_visited_time++;
			}

			//increase win time.
			inline void incr_win_time()
			{
				_win_time++;
			}

			//free the node from allocator.
			void FreeFromAllocator(Allocator& allocator)
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
					GADT_CHECK_WARNING(b == false, "MW105: free child node failed.");
				}
				else
				{
					allocator.free(this);
				}
			}

		public:
			MctsNode(const State& state) :
				_state(state),
				_visited_time(1),
				_win_time(0),
				_next_action_index(0)
			{
				_make_action(_state, _action_set);
				_child_nodes.resize(_action_set.size(), nullptr);
			}

			//TODO free child node from allocator by index. return true if free successfully.
			bool FreeChildNode(size_t index, Allocator& allocator)
			{
				//TODO
			}

			//4.the simulation result is back propagated through the selected nodes to update their statistics.
			void BackPropagation(const Result& result, const FuncPackage& func)
			{
				if (func.AllowUpdateValue(result, *this))
				{
					incr_win_time();
				}
			}

			//3.simulation is run from the new node according to the default policy to produce a result.
			void SimulationProcess(Result& result, const FuncPackage& func)
			{
				State state = _state;	//copy
				ActionSet actions;
				for (size_t i = 0;;i++)
				{
					if (is_debug){GADT_CHECK_WARNING(i > _default_policy_warning_length, "MW103: out of default policy process max length.");}
					AgentIndex winner = func.DetemineWinner(state);
					if (winner != _no_winner_index)
					{
						result = func.StateToResult(state, winner);
						break;
					}
					actions.clear();
					actions = func.MakeAction(state, actions);
					const Action& action = func.DefaultPolicy(actions);
					state = func.GetNewState(state, action);
				}

				BackPropagation(result, func);		//update the new value itself.
			}

			//2.one child node would be added to expand the tree, acccording to the available actions.
			void Expandsion(Result& result, Allocator& allocator ,const FuncPackage& func)
			{
				if (is_end_state())
				{
					result = func.StateToResult(_state, _winner_index);//return the result of this node.
					return;
				}
				else
				{
					Node* new_node = allocator.construct(func.GetNewState(_state, next_action()));
					set_next_child(new_node);
					to_next_action();
					new_node->SimulationProcess(result, func);
				}
			}

			//1. select the most urgent expandable node,and get the result to update statistic.
			void Selection(Result& result, Allocator& allocator,const FuncPackage& func)
			{
				incr_visited_time();

				if (exist_unactivated_action())
				{
					Expandsion(result, func);
				}
				else
				{
					Node* max_ucb_child_node = _child_nodes[0];
					UcbValue max_ucb_value = 0;
					for (Node* child_node : _child_nodes)
					{
						UcbValue child_node_ucb_value = func.TreePolicyValue(*this, *child_node);
						if (child_node_ucb_value > max_ucb_value)
						{
							max_ucb_child_node = child_node;
							max_ucb_value = child_node_ucb_value;
						}
					}
					max_ucb_child_node->Selection(result, func);
				}

				//backpropagation process for this node.update value;
				BackPropagation(result, func);
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
			using Node			= typename MctsNode<State, Action, Result, is_debug>;							
			using Allocator		= typename Node::Allocator;
			using ActionSet		= typename Node::ActionSet;								//ActionSet is the set of Action
			using NodePtrSet	= typename Node::NodePtrSet;								//ChildSet is the set of ptrs to child nodes.
			
		private:
			using FuncPackage	= typename Node::FuncPackage;
			struct DefaultFuncPackage
			{
				typename FuncPackage::TreePolicyValueFunc		TreePolicyValue;
				typename FuncPackage::DefaultPolicyFunc			DefaultPolicy;
				typename FuncPackage::AllowExtendFunc			AllowExtend;
				typename FuncPackage::AllowExcuteGcFunc			AllowExcuteGc;
				typename FuncPackage::ValueForRootNodeFunc		ValueForRootNode;
			};

		private:
			FuncPackage		_func_package;			//function package of the search.
			Allocator&		_allocator;				//the allocator for the search.
			const bool		_private_allocator;		//use private allocator.
			double			_timeout;				//set timeout (seconds).
			size_t			_max_iteration;			//set max iteration times.
			bool			_enable_gc;				//allow garbage collection if the tree run out of memory.
			
		public:
			//package of default functions.
			const DefaultFuncPackage DefaultFunc;

		private:
			//define functions as default.
			DefaultFuncPackage DefaultFuncInit()
			{
				auto TreePolicyValue = [](const Node& parent, const Node& child)->UcbValue{
					UcbValue avg = static_cast<UcbValue>(child.win_time()) / static_cast<UcbValue>(child.visited_time());
					return policy::UCB1(avg, static_cast<UcbValue>(parent.visited_time()), static_cast<UcbValue>(child.visited_time()));
				};
				auto DefaultPolicy = [](const ActionSet& actions)->const Action&{
					if (is_debug) 
					{
						GADT_CHECK_WARNING(actions.size() == 0, "MW104: empty action set during default policy.");
					}
					return actions[rand() % actions.size()];
				};
				auto AllowExtend = [](const Node& node)->bool {
					return true; 
				};
				auto AllowExcuteGc = [](const Node& node)->bool{
					if (node.visited_time() < 10)
					{
						return true;
					}
					return false;
				};
				auto ValueForRootNode = [](const Node& node)->UcbValue{
					return static_cast<UcbValue>(node.visited_time());
				};
				return {
					TreePolicyValue,
					DefaultPolicy,
					AllowExtend,
					AllowExcuteGc,
					ValueForRootNode
				};
			}

			void FuncInit()
			{
				_func_package.TreePolicyValue	= DefaultFunc.TreePolicyValue;
				_func_package.DefaultPolicy		= DefaultFunc.DefaultPolicy;
				_func_package.AllowExtend		= DefaultFunc.AllowExtend;
				_func_package.AllowExcuteGc		= DefaultFunc.AllowExcuteGc;
				_func_package.ValueForRootNode	= DefaultFunc.ValueForRootNode;
			}

			//excute iteration function.
			Action ExcuteMCTS(State root_state, double timeout, size_t max_iteration, bool enable_gc)
			{
				Node root_node(root_state);
				ActionSet root_actions = root_node.action_set();
				timer::TimePoint start_tp;
				size_t iteration_time = 0;
				for (iteration_time = 0; iteration_time < max_iteration; iteration_time++)
				{
					//stop search if timout.
					if (start_tp.time_since_created() > timeout)
					{
						break;//timeout, stop search.
					}

					//excute garbage collection if need.
					if (_allocator.is_full())
					{
						if (enable_gc)
						{
							//do garbage collection.

							if (_allocator.is_full())
							{
								break;//stop search if gc failed.
							}
						}
						else
						{
							break;//run out of memory, stop search.
						}
					}

					Result new_result;
					root_node.Selection(new_result, _allocator, _func_package);
				}

				//log info if is debuging.


				//return the best result
				if (is_debug) { GADT_CHECK_WARNING(root_actions.size() == 0, "MW101: root node do not exist any available action."); }
				UcbValue max_value = func.ValueForRootNode(*root_node.child_node(0));
				size_t max_value_node_index = 0;
				for (size_t i = 0; i < root_actions.size(); i++)
				{
					auto child_ptr = root_node.child_node(i);
					if (child_ptr != nullptr)
					{
						UcbValue child_value = _func_package.ValueForRootNode(*child_ptr);
						if (child_value > max_value)
						{
							max_value = child_value;
							max_value_node_index = i;
						}
					}
				}
				if (is_debug) { GADT_CHECK_WARNING(root_actions.size() == 0, "MW102: best value for root node equal to 0."); }
				return root_action[max_value_node_index];
			}

		public:
			//use private allocator.
			MctsSearch(
				typename FuncPackage::GetNewStateFunc		_GetNewState,
				typename FuncPackage::MakeActionFunc		_MakeAction,
				typename FuncPackage::DetemineWinnerFunc	_DetemineWinner,
				typename FuncPackage::StateToResultFunc		_StateToResult,
				typename FuncPackage::AllowUpdateValueFunc	_AllowUpdateValue,
				size_t max_node
			):
				_allocator(*(new Allocator(max_node))),
				_private_allocator(true),
				_func_package(
					_GetNewState,
					_MakeAction,
					_DetemineWinner,
					_StateToResult,
					_AllowUpdateValue
				),
				DefaultFunc(DefaultFuncInit())
			{
				FuncInit();
			}

			//use public allocator.
			MctsSearch(
				typename FuncPackage::GetNewStateFunc		_GetNewState,
				typename FuncPackage::MakeActionFunc		_MakeAction,
				typename FuncPackage::DetemineWinnerFunc	_DetemineWinner,
				typename FuncPackage::StateToResultFunc		_StateToResult,
				typename FuncPackage::AllowUpdateValueFunc	_AllowUpdateValue,
				Allocator allocator
			):
				_allocator(allocator),
				_private_allocator(false), 
				_func_package(
					_GetNewState,
					_MakeAction,
					_DetemineWinner,
					_StateToResult,
					_AllowUpdateValue
				),
				DefaultFunc(DefaultFuncInit())
			{
				FuncInit();
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
			Action DoMcts(const State root_state)
			{
				return ExcuteMCTS(root_state, _timeout, _max_iteration, _enable_gc);
			}

			//do search with custom parameters.
			Action DoMcts(const State root_state, double timeout, size_t max_iteration, bool enable_gc)
			{
				return ExcuteMCTS(root_state, timeout, max_iteration, enable_gc);
			}
		};
	}
}