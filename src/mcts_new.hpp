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
#include "visual_tree.h"

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
		template<typename T, bool _is_debug>
		class MctsAllocator
		{
		private:
			using pointer		= T*;
			using reference		= T&;
			using index_queue	= std::queue<size_t>;

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
				free(_fir_element);
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
			MctsAllocator(size_t count):
				_count(count),
				_available_index(),
				_fir_element(nullptr),
				_exist_element(nullptr)
			{
				allocate(count);
			}

			//copy constructor function.
			MctsAllocator(const MctsAllocator& target):
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
			~MctsAllocator()
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

		template<typename Action, bool _is_debug>
		class MctsActionListNode
		{
		public:
			using pointer = MctsActionListNode<Action, _is_debug>*;

		private:
			const Action _action;
			pointer      _next_node;

		public:
			//constructor function.
			inline MctsActionListNode(const Action& action):
				_action(action),
				_next_node(nullptr)
			{
			}

			//copy constructor function is disallowed.
			MctsActionListNode(const MctsActionListNode&) = delete;
			
			inline const Action& action() const { return _action; }
			inline pointer next_node() const { return _next_node; }
			inline void set_next_node(pointer p) { _next_node = p; }
			
		};

		template<typename Action, bool _is_debug>
		class MctsActionList
		{
		public:
			using ListNode = MctsActionListNode<Action, _is_debug>;
			using Allocator = MctsAllocator<ListNode, _is_debug>;
			using node_pointer = MctsActionListNode<Action, _is_debug>*;

		private:
			const bool   _private_allocator;
			Allocator&	 _allocator;
			node_pointer _first_node;
			node_pointer _last_node;
			node_pointer _iterator;

		public:
			MctsActionList(size_t allocator_count):
				_private_allocator(true),
				_allocator(*(new Allocator(allocator_count))),
				_first_node(nullptr),
				_last_node(nullptr),
				_iterator(nullptr)
			{
			}

			MctsActionList(Allocator& allocator):
				_private_allocator(false),
				_allocator(allocator),
				_first_node(nullptr),
				_last_node(nullptr),
				_iterator(nullptr)
			{
			}

			inline ~MctsActionList()
			{
				if (_private_allocator)
				{
					delete &_allocator;
				}
			}

			//insert a new value in the end of the list.
			void insert(const Action& action)
			{
				auto ptr = _allocator.construct(action);
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

			//get action from iterator.
			inline const Action& iterator() const
			{
				return _iterator->action();
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
		* MctsNode is the node class in the monte carlo tree search.
		*
		* [State] is the game-state class, which is defined by the user.
		* [Action] is the game-action class, which is defined by the user.
		* [Result] is the game-result class, which stand for a terminal state of the game.
		* [is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename State, typename Action, typename Result, bool _is_debug>
		class MctsNode
		{
		public:											
			using Node			= typename MctsNode<State, Action, Result, _is_debug>;		//MctsNode
			using Allocator		= typename MctsAllocator<Node, _is_debug>;					//MctsAllocate 
			using ActionSet		= typename std::vector<Action>;								//ActionSet is the set of Action.
			using NodePtrSet	= typename std::vector<MctsNode*>;							//ChildSet is the set of ptrs to child nodes.

			//function package
			struct FuncPackage
			{
			public:
				using GetNewStateFunc		= std::function<State(const State&, const Action&)>;
				using MakeActionFunc		= std::function<void(const State&, ActionSet&)>;	
				using DetemineWinnerFunc	= std::function<AgentIndex(const State&)>;			
				using StateToResultFunc		= std::function<Result(const State&, AgentIndex)>;	
				using AllowUpdateValueFunc	= std::function<bool(const State&, const Result&)>;	
				using TreePolicyValueFunc	= std::function<UcbValue(const Node&, const Node&)>;
				using DefaultPolicyFunc		= std::function<const Action&(const ActionSet&)>;	
				using AllowExtendFunc		= std::function<bool(const Node&)>;					
				using AllowExcuteGcFunc		= std::function<bool(const Node&)>;					
				using ValueForRootNodeFunc	= std::function<UcbValue(const Node&)>;				

			public:
				//necessary functions.
				const GetNewStateFunc		GetNewState;		//get a new state from previous state and action.
				const MakeActionFunc		MakeAction;			//the function which create action set by the state.
				const DetemineWinnerFunc	DetemineWinner;		//return no_winner_index if a state is not terminal state.
				const StateToResultFunc		StateToResult;		//get a result from state and winner.
				const AllowUpdateValueFunc	AllowUpdateValue;	//update values in the node by the result.

				//default functions.
				TreePolicyValueFunc			TreePolicyValue;	//value of child node in selection process. the highest would be seleced.
				DefaultPolicyFunc			DefaultPolicy;		//the default policy to select action.
				AllowExtendFunc				AllowExtend;		//allow node to extend child node.
				AllowExcuteGcFunc			AllowExcuteGc;		//the condition to excute gc in a node.
				ValueForRootNodeFunc		ValueForRootNode;	//select best action of root node after iterations finished.

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
					ValueForRootNodeFunc	_ValueForRootNode
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
			const size_t		child_num()				const { return _child_nodes.size(); }
			const NodePtrSet&	child_set()				const { return _child_nodes; }
			const MctsNode*		child_node(size_t i)	const { return _child_nodes[i]; }
			const size_t		action_num()			const { return _action_set.size(); }
			const ActionSet&	action_set()			const { return _action_set; }
			const Action&		action(size_t i)		const { return _action_set[i]; }

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
				return _action_set[_next_action_index];
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
				if (is_debug())
				{
					bool b = allocator.free(this);
					GADT_CHECK_WARNING(b == false, "MCTS105: free child node failed.");
				}
				else
				{
					allocator.free(this);
				}
			}

		public:
			MctsNode(const State& state, const FuncPackage& func) :
				_state(state),
				_winner_index(func.DetemineWinner(state)),
				_visited_time(1),
				_win_time(0),
				_next_action_index(0)
			{
				if (!is_end_state())
				{
					func.MakeAction(_state, _action_set);
					_child_nodes.resize(_action_set.size(), nullptr);
				}
			}

			MctsNode(const MctsNode&) = delete;

			//TODO free child node from allocator by index. return true if free successfully.
			bool FreeChildNode(size_t index, Allocator& allocator)
			{
				//TODO
			}

			//4.the simulation result is back propagated through the selected nodes to update their statistics.
			void BackPropagation(const Result& result, const FuncPackage& func)
			{
				if (func.AllowUpdateValue(_state, result))
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
					if (is_debug()){GADT_CHECK_WARNING(i > _default_policy_warning_length, "MCTS103: out of default policy process max length.");}
					AgentIndex winner = func.DetemineWinner(state);
					if (winner != _no_winner_index)
					{
						result = func.StateToResult(state, winner);
						break;
					}
					actions.clear();
					func.MakeAction(state, actions);
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
					Node* new_node = allocator.construct(func.GetNewState(_state, next_action()),func);
					set_next_child(new_node);
					to_next_action();
					new_node->SimulationProcess(result, func);
				}
			}

			//1. select the most urgent expandable node,and get the result to update statistic.
			void Selection(Result& result, Allocator& allocator,const FuncPackage& func)
			{
				incr_visited_time();

				if (is_end_state())
				{
					func.StateToResult(_state, result);
				}
				else
				{
					if (exist_unactivated_action())
					{
						Expandsion(result, allocator, func);
					}
					else
					{
						if (is_debug()) { GADT_CHECK_WARNING(_child_nodes.size() == 0, "MCTS106: empty action set during tree policy."); }
						Node* max_ucb_child_node = _child_nodes[0];
						UcbValue max_ucb_value = 0;
						for (size_t i = 0; i < _child_nodes.size(); i++)
						{
							if (_child_nodes[i] != nullptr)
							{
								UcbValue child_node_ucb_value = func.TreePolicyValue(*this, *_child_nodes[i]);
								if (child_node_ucb_value > max_ucb_value)
								{
									max_ucb_child_node = _child_nodes[i];
									max_ucb_value = child_node_ucb_value;
								}
							}
						}
						if (is_debug()) { GADT_CHECK_WARNING(max_ucb_child_node == nullptr, "MCTS108: best child node pointer is nullptr."); }
						max_ucb_child_node->Selection(result, allocator, func);
					}
				}

				//backpropagation process for this node.update value;
				BackPropagation(result, func);
			}

			//return true if the state is the terminal-state of the game.
			inline bool is_end_state() const
			{
				return _winner_index != _no_winner_index;
			}

			//get info of this node.
			std::string info() const
			{
				std::stringstream ss;
				double avg = static_cast<double>(win_time()) / static_cast<double>(visited_time());
				ss << "{ visited:" << visited_time() << " win:" << win_time() <<" avg:" << avg << " child";
				if (exist_unactivated_action())
				{
					ss << next_action_index() << "/" << action_set().size();
				}
				else
				{
					ss << child_set().size() << "/" << child_set().size();
				}
				ss << " }";
				return ss.str();
			}

			//return the value of _is_debug.
			constexpr inline bool is_debug() const
			{
				return _is_debug;
			}
		};

		//convert mcts search tree to json.
		template<typename State, typename Action, typename Result, bool _is_debug>
		class MctsToJson
		{
		public:
			using SearchNode     = MctsNode<State, Action, Result, _is_debug>;
			using VisualTree     = visual_tree::VisualTree;
			using VisualNode	 = visual_tree::VisualNode;
			using StateToStrFunc = std::function<std::string(const State& state)>;
			using CustomInfoFunc = std::function<void(const SearchNode&, VisualNode&)>;

		private:
			const char* DEPTH_NAME           = "depth";
			const char* COUNT_NAME           = "tree_size";
			const char* STATE_NAME           = "state";
			const char* WINNER_INDEX_NAME    = "winner";
			const char* VISITED_TIME_NAME    = "visited_time";
			const char* WIN_TIME_NAME        = "win_time";
			const char* CHILD_NUM_NAME       = "child_number";
			const char* IS_TERMIANL_NAME     = "is_terminal";
			
		private:
			SearchNode*    _mcts_root_node;
			VisualTree     _json_tree;
			bool           _include_state;
			StateToStrFunc _StateToStr;
			CustomInfoFunc _CustomInfo;

		private:
			//search node convert to json node.
			void convert_node(const SearchNode& search_node, VisualNode& visual_node)
			{
				visual_node.add_value(DEPTH_NAME, visual_node.depth());
				visual_node.add_value(WINNER_INDEX_NAME, search_node.winner_index());
				visual_node.add_value(VISITED_TIME_NAME, search_node.visited_time());
				visual_node.add_value(WIN_TIME_NAME, search_node.win_time());
				visual_node.add_value(CHILD_NUM_NAME, search_node.child_num());
				visual_node.add_value(IS_TERMIANL_NAME, search_node.is_end_state());
				if (_include_state)
				{
					visual_node.add_value(STATE_NAME, _StateToStr(search_node.state()));
				}
				_CustomInfo(search_node, visual_node);
				for (size_t i = 0;i<search_node.child_num();i++)
				{
					auto node_ptr = search_node.child_node(i);
					if (node_ptr != nullptr)
					{
						visual_node.create_child();
						convert_node(*search_node.child_node(i), *visual_node.last_child());
					}
				}
			}

			//add count
			void AddCount(VisualNode& node)
			{
				node.add_value(COUNT_NAME, node.count());
			}

			//search tree convert to json tree.
			void ConvertToVisualTree(SearchNode* mcts_root_node,VisualTree& visual_tree)
			{
				visual_tree.clear();
				convert_node(*mcts_root_node, *visual_tree.root_node());//generate new visual tree.
				visual_tree.traverse_nodes([&](VisualNode& node)->void {AddCount(node); });	//refresh count value.
			}

		public:
			//constructor function.
			MctsToJson(SearchNode* mcts_root_node, StateToStrFunc StateToStr, CustomInfoFunc CustomInfo = [](const SearchNode&, VisualNode&)->void {}) :
				_mcts_root_node(mcts_root_node),
				_json_tree(),
				_include_state(true),
				_StateToStr(StateToStr),
				_CustomInfo(CustomInfo)
			{
				ConvertToVisualTree(mcts_root_node, _json_tree);
			}

			MctsToJson(SearchNode* mcts_root_node):
				_mcts_root_node(mcts_root_node),
				_json_tree(),
				_include_state(false),
				_StateToStr([](const State&)->std::string { return ""; }),
				_CustomInfo([](const SearchNode&, VisualNode&)->void {})
			{
				ConvertToVisualTree(mcts_root_node, _json_tree);
			}

			//refresh json tree by mcts search tree.
			inline void refresh()
			{
				_json_tree = ConvertToVisualTree(_mcts_root_node);
			}

			//set custom info function.
			inline void set_custom_info(CustomInfoFunc CustomInfo)
			{
				_CustomInfo = CustomInfo;
			}

			//output json.
			inline void output_json(std::ostream& os)
			{
				_json_tree.output_json(os);
			}
		};

		/*
		* MctsSearch is a template of monte carlo tree search.
		*
		* [State] is the game-state class, which is defined by the user.
		* [Action] is the game-action class, which is defined by the user.
		* [Result] is the game-result class, which stand for a terminal state of the game.
		* [_is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename State, typename Action, typename Result, bool _is_debug = false>
		class MctsSearch
		{
		public:
			using Node       = typename MctsNode<State, Action, Result, _is_debug>;	  //searcg node.	
			using VisualTree = typename MctsToJson<State, Action, Result, _is_debug>; //json tree
			using Allocator  = typename Node::Allocator;							  //allocator of nodes
			using ActionSet  = typename Node::ActionSet;							  //set of Action
			using NodePtrSet = typename Node::NodePtrSet;							  //set of ptrs to child nodes.
			
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
			struct LogFuncPackage
			{
				using StateToStrFunc	= std::function<std::string(const State& state)>;
				using ActionToStrFunc	= std::function<std::string(const Action& action)>;
				using ResultToStrFunc	= std::function<std::string(const Result& result)>;

				StateToStrFunc		StateToStr;
				ActionToStrFunc		ActionToStr;
				ResultToStrFunc		ResultToStr;
			};

		private:
			FuncPackage		_func_package;			//function package of the search.
			Allocator&		_allocator;				//the allocator for the search.
			const bool		_private_allocator;		//use private allocator.
			double			_timeout;				//set timeout (seconds).
			size_t			_max_iteration;			//set max iteration times.
			bool			_enable_gc;				//allow garbage collection if the tree run out of memory.
			bool			_enable_log;			//enable log visualization.
			std::ostream*	_log_ptr;				//pointer to log ostream.

			const char* MCTS_JSON_LOG_NAME = "MctsJsonLog.txt";

		public:
			//package of default functions.
			const DefaultFuncPackage DefaultFunc;
			LogFuncPackage LogFunc;

		private:

			//get reference of log ostream
			inline std::ostream& log()
			{
				return *_log_ptr;
			}

			//get info of this search.
			std::string info() const
			{
				std::stringstream ss;
				ss << std::boolalpha << "{" << std::endl
					<< "    allocator: " << _allocator.info() << std::endl
					<< "    is_private_allocator: " << _private_allocator << std::endl
					<< "    timeout: " << _timeout << std::endl
					<< "    max_iteration: " << _max_iteration << std::endl
					<< "    enable_gc: " << _enable_gc << std::endl
					<< "    enable_log: " << _enable_log << std::endl
					<< "}" << std::endl;
				return ss.str();
			}

			//enable log
			inline bool enable_log() const
			{
				return _enable_log;
			}

			//enable gc
			inline bool enable_gc() const
			{
				return _enable_gc;
			}

			//define functions as default.
			DefaultFuncPackage DefaultFuncInit()
			{
				auto TreePolicyValue = [](const Node& parent, const Node& child)->UcbValue{
					UcbValue avg = static_cast<UcbValue>(child.win_time()) / static_cast<UcbValue>(child.visited_time());
					return policy::UCB1(avg, static_cast<UcbValue>(parent.visited_time()), static_cast<UcbValue>(child.visited_time()));
				};
				auto DefaultPolicy = [](const ActionSet& actions)->const Action&{
					if (_is_debug) 
					{
						GADT_CHECK_WARNING(actions.size() == 0, "MCTS104: empty action set during default policy.");
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

			//function package initilize.
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
				if (_enable_log)
				{
					log() << "[MCTS] start excute monte carlo tree search..." << std::endl
						<< "[MCTS] info = " << info() << std::endl;
				}
				Node* root_node = _allocator.construct(root_state, _func_package);
				ActionSet root_actions = root_node->action_set();
				timer::TimePoint start_tp;
				size_t iteration_time = 0;
				for (iteration_time = 0; iteration_time < max_iteration; iteration_time++)
				{
					//stop search if timout.
					if (start_tp.time_since_created() > timeout && is_debug() == false)
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
					root_node->Selection(new_result, _allocator, _func_package);
				}

				//return the best result
				if (is_debug()) { GADT_CHECK_WARNING(root_actions.size() == 0, "MCTS101: root node do not exist any available action."); }
				UcbValue max_value = 0;
				size_t max_value_node_index = 0;
				if (_enable_log) 
				{ 
					VisualTree json_tree(root_node, LogFunc.StateToStr);
					json_tree.output_json(std::ofstream(MCTS_JSON_LOG_NAME));
					log() << "[MCTS] iteration finished." << std::endl
						<< "[MCTS] actions = {" << std::endl;
				}
				for (size_t i = 0; i < root_actions.size(); i++)
				{
					auto child_ptr = root_node->child_node(i);
					if (is_debug()) { GADT_CHECK_WARNING(root_node->child_node(0) == nullptr, "MCTS107: empty child node under root node."); }
					if (_enable_log)
					{
						log() << "action " << i << ": "<< LogFunc.ActionToStr(root_actions[i])<<", value: ";
					}
					if (child_ptr != nullptr)
					{
						UcbValue child_value = _func_package.ValueForRootNode(*child_ptr);
						if (child_value > max_value)
						{
							max_value = child_value;
							max_value_node_index = i;
						}
						if (_enable_log)
						{
							log() << "[" << child_value << "]" << std::endl;
						}
					}
					else
					{
						if (_enable_log)
						{
							log() << "[ deleted ]" << std::endl;
						}
					}
				}
				if (_enable_log)
				{
					log() << "[MCTS] best action index: "<< max_value_node_index << std::endl;
				}
				if (is_debug()) { GADT_CHECK_WARNING(root_actions.size() == 0, "MCTS102: best value for root node equal to 0."); }
				return root_actions[max_value_node_index];
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
				_func_package(
					_GetNewState,
					_MakeAction,
					_DetemineWinner,
					_StateToResult,
					_AllowUpdateValue
				),
				_allocator(*(new Allocator(max_node))),
				_private_allocator(true),
				_enable_log(false),
				_log_ptr(&std::cout),
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
				_func_package(
					_GetNewState,
					_MakeAction,
					_DetemineWinner,
					_StateToResult,
					_AllowUpdateValue
				),
				_allocator(allocator),
				_private_allocator(false), 
				_enable_log(false),
				_log_ptr(&std::cout),
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

			//enable log output to ostream.
			inline void EnableLog(
				typename LogFuncPackage::StateToStrFunc     StateToStr,
				typename LogFuncPackage::ActionToStrFunc    ActionToStr,
				typename LogFuncPackage::ResultToStrFunc    ResultToStr,
				std::ostream& log = std::cout
			)
			{
				LogFunc = { StateToStr,ActionToStr,ResultToStr };
				_enable_log = true;
				_log_ptr = &log;
			}

			inline void DisableLog()
			{
				_enable_log = false;
			}

			//return the value of _is_debug.
			constexpr inline bool is_debug() const
			{
				return _is_debug;
			}
		};

	}
}