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

#include "gadtlib.h"
#include "visual_tree.h"
#include "gadtstl.hpp"

#pragma once

namespace gadt
{
	namespace mcts_new
	{
		//allow check warning if it is true.
		constexpr const bool g_MCTS_NEW_ENABLE_WARNING = true;

		//the method for parallel mcts
		enum MctsParalleMethod: uint8_t
		{
			NO_PARALLELIZATION = 0,
			LEAF_PARALLELIZATION = 1,
			ROOT_PARALLELIZATION = 2,
			TREE_PARALLELIZATION = 3
		};

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
		* MctsSetting is the setting of MCTS.
		*
		* MctsSetting() would use default setting.
		* MctsSetting(params) would generate custom setting.
		*/
		struct MctsSetting
		{
			double		timeout;					//set timeout (seconds).
			size_t		thread_num;					//thread num.
			size_t		max_iteration_per_thread;	//set max iteration times.
			size_t		max_node_per_thread;		//pre-allocated memory for each thread.
			AgentIndex	no_winner_index;			//agent index of no winner.
			size_t		simulation_warning_length;	//if the simulation length out of this value, it would throw a warning if is debug.

			//default setting constructor.
			MctsSetting() :
				timeout(30),
				thread_num(1),
				max_iteration_per_thread(10000),
				max_node_per_thread(10000),
				no_winner_index(0),
				simulation_warning_length(1000)
			{
			}

			//custom setting constructor.
			MctsSetting(
				double _timeout, 
				size_t _thread_num,	
				size_t _max_iteration_per_thread,
				bool _max_node_per_thread,
				AgentIndex _no_winner_index = 0,
				size_t _simulation_warning_length = 1000
			) :
				timeout(_timeout),
				thread_num(_thread_num),
				max_iteration_per_thread(_max_iteration_per_thread),
				max_node_per_thread(_max_node_per_thread),
				no_winner_index(_no_winner_index),
				simulation_warning_length(_simulation_warning_length)
			{
			}

			//output print with str behind each line.
			std::string info() const
			{
				table::ConsoleTable tb(2, 6);
				tb.set_width({ 12,6 });
				tb.enable_title({"MCTS SETTING"});
				tb.set_cell_in_row(0, { { "timeout" },					{ console::ToString(timeout) } });
				tb.set_cell_in_row(1, { { "thread_num" },				{ console::ToString(thread_num) } });
				tb.set_cell_in_row(2, { { "max_iteration_per_thread" },	{ console::ToString(max_iteration_per_thread) } });
				tb.set_cell_in_row(3, { { "max_node_per_thread" },		{ console::ToString(max_node_per_thread) } });
				tb.set_cell_in_row(4, { { "no_winner_index" },			{ console::ToString(no_winner_index) } });
				tb.set_cell_in_row(5, { { "simulation_warning_length" },{ console::ToString(simulation_warning_length) } });
				return tb.output_string();
			}
		};

		/*
		* MctsFuncPackage include all necessary functions for MCTS.
		*
		* use MctsFuncPackage(...) to construct a new function package. 
		*/
		template<typename State, typename Action, typename Result, bool _is_debug>
		struct MctsFuncPackage;

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
			using Node			= MctsNode<State, Action, Result, _is_debug>;					//MctsNode
			using pointer       = Node*;
			using reference     = Node&;
			using Allocator		= gadt::stl::StackAllocator<Node, _is_debug>;			//Allocate 
			using ActionSet		= std::vector<Action>;									//ActionSet is the set of Action.
			using FuncPackage	= MctsFuncPackage<State, Action, Result, _is_debug>;	

		private:
			State			_state;				//state of this node.
			AgentIndex		_winner_index;		//the winner index of the state.
			uint32_t		_visited_time;		//how many times that this node had been visited.
			uint32_t		_win_time;			//win time accmulated by the simulation.
			ActionSet		_action_set;		//action set of this node.
			
			pointer			_parent_node;		//pointer to parent node
			pointer			_fir_child_node;	//pointer to left most child node.
			pointer			_brother_node;		//pointer to breother node.

		public:
			const State&        state()               const { return _state; }
			const ActionSet&    action_set()          const { return _action_set; }
			const Action&       action(size_t i)      const { return _action_set[i]; }
			size_t              action_num()          const { return _action_set.size(); }
			AgentIndex          winner_index()        const { return _winner_index; }
			uint32_t            visited_time()        const { return _visited_time; }
			uint32_t            win_time()            const { return _win_time; }

			pointer             parent_node()         const { return _parent_node; }
			pointer             fir_child_node()      const { return _fir_child_node; }
			pointer				brother_node()        const { return _brother_node; }

			bool                exist_parent_node()   const { return _parent_node != nullptr; }
			bool                exist_child_node()    const { return _fir_child_node != nullptr; }
			bool			    exist_brother_node()  const { return _brother_node != nullptr; }
			
		private:
			//a value means no winner, which is differ from any other AgentIndex.
			static const AgentIndex _no_winner_index = 0;
			static const size_t		_default_policy_warning_length = 1000;

			//exist unactived action in the action set.
			inline bool exist_unactivated_action() const
			{
				if (_action_set.size() == 0)
					return false;
				else if (_action_set.size() == child_num())
					return false;
				return true;
			}

			//get next action.
			inline const Action& next_action()
			{
				return _action_set[child_num()];
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
				if (brother_node() != nullptr)
				{
					brother_node()->FreeFromAllocator(allocator);
				}

				if (fir_child_node() != nullptr)
				{
					fir_child_node()->FreeFromAllocator(allocator);
				}

				//free the node itself.
				if (is_debug())
				{
					bool b = allocator.destory(this);
					GADT_CHECK_WARNING(g_MCTS_NEW_ENABLE_WARNING, b == false, "MCTS105: free child node failed.");
				}
				else
				{
					allocator.destory(this);
				}
			}

		public:
			MctsNode(const State& state, pointer parent_node, const FuncPackage& func) :
				_state(state),
				_winner_index(func.DetemineWinner(state)),
				_visited_time(1),
				_win_time(0),
				_parent_node(parent_node),
				_fir_child_node(nullptr),
				_brother_node(nullptr)
			{
				if (!is_end_state())
				{
					func.MakeAction(_state, _action_set);
				}
			}

			MctsNode(const MctsNode&) = delete;

			//TODO
			//TODO
			//TODO free child node from allocator by index. return true if free successfully.
			bool FreeChildNode(size_t index, Allocator& allocator)
			{
				//TODO
				return true;
			}

			//4.the simulation result is back propagated through the selected nodes to update their statistics.
			void BackPropagation(const Result& result, const FuncPackage& func)
			{
				//update node if allow.
				if (func.AllowUpdateValue(_state, result))
				{
					incr_win_time();
				}

				//update parent node.
				if (exist_parent_node())
				{
					parent_node()->BackPropagation(result, func);
				}
			}

			//3.simulation is run from the new node according to the default policy to produce a result.
			Result SimulationProcess(const FuncPackage& func, const MctsSetting& setting)
			{
				State state = _state;	//copy
				ActionSet actions;
				for (size_t i = 0;; i++)
				{
					GADT_CHECK_WARNING(is_debug(), i > setting.simulation_warning_length, "MCTS103: out of default policy process max length.");

					//detemine winner
					AgentIndex winner = func.DetemineWinner(state);

					//return result if exist.
					if (winner != setting.no_winner_index)
					{
						return func.StateToResult(state, winner);
					}

					//generate new actions.
					actions.clear();
					func.MakeAction(state, actions);

					//choose action by default policy.
					const Action& action = func.DefaultPolicy(actions);

					//state update.
					state = func.GetNewState(state, action);
				}
				return func.StateToResult(_state, _no_winner_index);
			}

			//2.one child node would be added to expand the tree, acccording to the available actions.
			void Expandsion(Allocator& allocator ,const FuncPackage& func, const MctsSetting& setting)
			{
				if (is_end_state())
				{
					Result result = func.StateToResult(_state, _winner_index);//return the result of this node.
					BackPropagation(result, func);
					return; //back propagate from this node to root.
				}
				else
				{
					volatile const size_t new_child_index = child_num();
					if (new_child_index < _action_set.size())
					{
						//create new node.
						pointer new_node = allocator.construct(func.GetNewState(_state, _action_set[new_child_index]), this, func);
						Result result = new_node->SimulationProcess(func, setting);
						new_node->_parent_node = this;

						//link to father. return if link failed.
						if (!link_as_child(new_child_index, new_node)) { return; }
						new_node->BackPropagation(result, func);
					}
					return; //fialed to find the next expandable child node, return;
				}
			}

			//1. select the most urgent expandable node,and get the result to update statistic.
			void Selection(Allocator& allocator,const FuncPackage& func, const MctsSetting& setting)
			{
				incr_visited_time();

				if (is_end_state())
				{
					Result result = func.StateToResult(_state, _winner_index);
					BackPropagation(result, func);
				}
				else
				{
					if (exist_unactivated_action())
					{
						Expandsion(allocator, func, setting);
					}
					else
					{
						GADT_CHECK_WARNING(is_debug(), _action_set.size() == 0, "MCTS106: empty action set during tree policy.");

						pointer max_ucb_child_node = fir_child_node();
						UcbValue max_ucb_value = 0;

						pointer node = fir_child_node();
						while(node != nullptr)
						{
							UcbValue child_node_ucb_value = func.TreePolicyValue(*this, *node);
							if (child_node_ucb_value > max_ucb_value)
							{
								max_ucb_child_node = node;
								max_ucb_value = child_node_ucb_value;
							}
							node = node->brother_node();
						}
						GADT_CHECK_WARNING(is_debug(), max_ucb_child_node == nullptr, "MCTS108: best child node pointer is nullptr.");
						max_ucb_child_node->Selection(allocator, func, setting);
					}
				}
			}

			//return true if the state is the terminal-state of the game.
			inline bool is_end_state() const
			{
				return _winner_index != _no_winner_index;
			}

			//get child num
			size_t child_num() const
			{
				size_t num = 0;
				if (exist_child_node())
				{
					Node* node = fir_child_node();
					num++;
					while (node->exist_brother_node())
					{
						node = node->brother_node();
						num++;
					}
				}
				return num;
			}

			//get child node by index.
			bool link_as_child(size_t index, pointer node)
			{
				if (index == 0)
				{
					_fir_child_node = node;
				}
				else
				{
					pointer* p = &_fir_child_node;
					for (size_t i = 0; i < index; i++)
					{
						if ((*p) == nullptr)
						{
							return false;
						}
						p = &((*p)->_brother_node);
					}
					*p = node;
				}
				return true;
			}

			//get info of this node.
			std::string info() const
			{
				std::stringstream ss;
				double avg = static_cast<double>(win_time()) / static_cast<double>(visited_time());
				ss << "{ visited:" << visited_time() << " win:" << win_time() <<" avg:" << avg << " child";
				if (exist_unactivated_action())
				{
					ss << child_num() << "/" << action_set().size();
				}
				else
				{
					ss << action_set().size() << "/" << action_set().size();
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

		//function package
		template<typename State, typename Action, typename Result, bool _is_debug>
		struct MctsFuncPackage
		{
		private:
			using Node = MctsNode<State, Action, Result, _is_debug>;
			using ActionSet = std::vector<Action>;

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
			MctsFuncPackage(
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
			) :
				GetNewState(_GetNewState),
				MakeAction(_MakeAction),
				DetemineWinner(_DetemineWinner),
				StateToResult(_StateToResult),
				AllowUpdateValue(_AllowUpdateValue),
				TreePolicyValue(_TreePolicyValue),
				DefaultPolicy(_DefaultPolicy),
				AllowExtend(_AllowExtend),
				AllowExcuteGc(_AllowExcuteGc),
				ValueForRootNode(_ValueForRootNode)
			{
			}

			MctsFuncPackage(
				GetNewStateFunc			_GetNewState,
				MakeActionFunc			_MakeAction,
				DetemineWinnerFunc		_DetemineWinner,
				StateToResultFunc		_StateToResult,
				AllowUpdateValueFunc	_AllowUpdateValue
			) :
				GetNewState(_GetNewState),
				MakeAction(_MakeAction),
				DetemineWinner(_DetemineWinner),
				StateToResult(_StateToResult),
				AllowUpdateValue(_AllowUpdateValue),
				TreePolicyValue([](const Node& parent, const Node& child)->UcbValue {
					UcbValue avg = static_cast<UcbValue>(child.win_time()) / static_cast<UcbValue>(child.visited_time());
					return policy::UCB1(avg, static_cast<UcbValue>(parent.visited_time()), static_cast<UcbValue>(child.visited_time()));
				}),
				DefaultPolicy([](const ActionSet& actions)->const Action&{
					if (_is_debug)
					{
						GADT_CHECK_WARNING(g_MCTS_NEW_ENABLE_WARNING, actions.size() == 0, "MCTS104: empty action set during default policy.");
					}
					return actions[rand() % actions.size()];
				}),
				AllowExtend([](const Node& node)->bool {
					return true;
				}),
				AllowExcuteGc([](const Node& node)->bool {
					if (node.visited_time() < 10)
					{
						return true;
					}
					return false;
				}),
				ValueForRootNode([](const Node& node)->UcbValue {
					return static_cast<UcbValue>(node.visited_time());
				})
			{
			}
		};

		/*
		* MctsToJson is used for convert mcts search tree to json tree.
		*
		* [State] is the game-state class, which is defined by the user.
		* [Action] is the game-action class, which is defined by the user.
		* [Result] is the game-result class, which stand for a terminal state of the game.
		*/
		template<typename State, typename Action, typename Result, bool _is_debug>
		class MctsJsonConvertor
		{
		public:
			using SearchNode     = MctsNode<State, Action, Result, _is_debug>;
			using VisualTree     = visual_tree::VisualTree;
			using VisualNode	 = visual_tree::VisualNode;
			using StateToStrFunc = std::function<std::string(const State& state)>;

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
			VisualTree*    _visual_tree;
			StateToStrFunc _StateToStr;

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
				visual_node.add_value(STATE_NAME, _StateToStr(search_node.state()));
				auto node_ptr = search_node.fir_child_node();
				while(node_ptr != nullptr)
				{
					visual_node.create_child();
					convert_node(*node_ptr, *visual_node.last_child());
					node_ptr = node_ptr->brother_node();
				}
			}

			//add count
			void AddCount(VisualNode& node)
			{
				node.add_value(COUNT_NAME, node.count());
			}

			//search tree convert to json tree.
			void ConvertToVisualTree()
			{
				_visual_tree->clear();
				convert_node(*_mcts_root_node, *_visual_tree->root_node());//generate new visual tree.
				_visual_tree->traverse_nodes([&](VisualNode& node)->void {AddCount(node); });	//refresh count value.
			}

		public:
			//constructor function.
			MctsJsonConvertor(SearchNode* mcts_root_node,VisualTree* visual_tree, StateToStrFunc StateToStr) :
				_mcts_root_node(mcts_root_node),
				_visual_tree(visual_tree),
				_StateToStr(StateToStr)
			{
				ConvertToVisualTree();
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
		class MonteCarloTreeSearch
		{
		public:
			using Node			= MctsNode<State, Action, Result, _is_debug>;			//searcg node.	
			using ActionSet		= typename Node::ActionSet;								//set of Action

		private:
			using Allocator     = typename Node::Allocator;								//allocator of nodes
			using LogController = log::SearchLogger<State, Action, Result>;				//log controller
			using JsonConvert   = MctsJsonConvertor<State, Action, Result, _is_debug>;	//json tree
			using FuncPackage   = MctsFuncPackage<State, Action, Result, _is_debug>;	//function package.

		private:
			FuncPackage		_func_package;			//function package of the search.
			MctsSetting		_setting;			//monte carlo tree search setting.
			LogController	_log_controller;		//controller of the logs.

		private:

			//return true if is debug.
			constexpr bool is_debug() const
			{
				return _is_debug;
			}

			//get reference of log ostream
			inline std::ostream& logger()
			{
				return _log_controller.log_ostream();
			}

			//return true if log enabled.
			inline bool log_enabled() const
			{
				return _log_controller.log_enabled();
			}

			//return true if json output enabled.
			inline bool json_output_enabled() const
			{
				return _log_controller.json_output_enabled();
			}

			//excute iteration function.
			Action ExcuteMCTS(State root_state)
			{
				//outputt log.
				if (log_enabled())
				{
					logger() << "[ Monte Carlo Tree Search ]" << std::endl;
					logger() << _setting.info();
					logger() << std::endl << ">> Executing MCTS......" << std::endl;
				}

				//initialized
				Node root_node(root_state, nullptr, _func_package);
				ActionSet root_actions = root_node.action_set();
				

				//thread content
				std::vector<Allocator> allocators;
				std::vector<std::thread> threads;

				//enable multithread.
				for (size_t thread_id = 0; thread_id < _setting.thread_num; thread_id++)
				{
					allocators.push_back(std::move(Allocator(_setting.max_node_per_thread)));
					threads.push_back(std::thread([](Node* root_node, Allocator* allocator, FuncPackage func, MctsSetting setting)->void {
						timer::TimePoint start_tp;
						size_t iteration_time = 0;
						for (iteration_time = 0; iteration_time < setting.max_iteration_per_thread; iteration_time++)
						{
							//stop search if timout.
							if (start_tp.time_since_created() > setting.timeout && setting.timeout != 0)
							{
								break;//timeout, stop search.
							}

							//excute garbage collection if need.
							if (allocator->is_full())
							{
								break;//run out of memory, stop search.
							}

							//excute next.
							root_node->Selection(*allocator, func, setting);
						}
					}, &root_node, &allocators[thread_id], _func_package, _setting));
				}

				for (size_t i = 0; i < threads.size(); i++)
				{
					threads[i].join();
				}

				//return the best result
				GADT_CHECK_WARNING(is_debug(), root_actions.size() == 0, "MCTS101: root node do not exist any available action.");

				//output Json if enabled.
				if (json_output_enabled())
				{
					JsonConvert(&root_node, &_log_controller.visual_tree(), _log_controller.state_to_str_func());
					_log_controller.OutputJson();
				}

				//select best action.
				UcbValue max_value = 0;
				size_t max_value_node_index = 0;
				Node* child_ptr = root_node.fir_child_node();
				std::vector<UcbValue> child_value_set(root_node.action_set().size());

				GADT_CHECK_WARNING(is_debug(), root_node.fir_child_node() == nullptr, "MCTS107: empty child node under root node.");
				for (size_t i = 0; i < root_actions.size(); i++)
				{
					if (child_ptr != nullptr)
					{
						child_value_set[i] = _func_package.ValueForRootNode(*child_ptr);
						if (child_value_set[i] > max_value)
						{
							max_value = child_value_set[i];
							max_value_node_index = i;
						}
					}
					child_ptr = child_ptr->brother_node();
					if (child_ptr == nullptr) { break; }
				}

				if (log_enabled())
				{
					table::ConsoleTable tb(4, root_node.action_set().size() + 1);
					tb.enable_title({ "MCTS RESULT" });
					tb.set_cell_in_row(0, { { "Index" },{ "Action" },{ "Value" },{ "Is Best" } });
					tb.set_width({ 3,10,4,4 });
					for (size_t i = 0; i < root_node.action_set().size(); i++)
					{
						tb.set_cell_in_row(i + 1, {
							{ console::ToString(i) },
							{ _log_controller.action_to_str_func()(root_node.action_set()[i]) },
							{ console::ToString(child_value_set[i]) },
							{ i == max_value_node_index ? "Yes " : "  " }
						});
					}
					logger() << tb.output_string(true, false) << std::endl;
				}

				GADT_CHECK_WARNING(is_debug(), root_actions.size() == 0, "MCTS102: best value for root node equal to 0.");

				return root_actions[max_value_node_index];
			}

		public:
			//use private allocator.
			MonteCarloTreeSearch(
				typename FuncPackage::GetNewStateFunc		_GetNewState,
				typename FuncPackage::MakeActionFunc		_MakeAction,
				typename FuncPackage::DetemineWinnerFunc	_DetemineWinner,
				typename FuncPackage::StateToResultFunc		_StateToResult,
				typename FuncPackage::AllowUpdateValueFunc	_AllowUpdateValue
			):
				_func_package(
					_GetNewState,
					_MakeAction,
					_DetemineWinner,
					_StateToResult,
					_AllowUpdateValue
				),
				_setting(),
				_log_controller()
			{
			}


			//use private allocator.
			MonteCarloTreeSearch(FuncPackage function_package, size_t max_node) :
				_func_package(function_package),
				_setting(),
				_log_controller()
			{
			}

			//deconstructor function
			~MonteCarloTreeSearch()
			{
			}

			//do search with custom setting.
			inline Action DoMcts(const State root_state, MctsSetting setting = MctsSetting())
			{
				_setting = setting;
				return ExcuteMCTS(root_state);
			}

			//enable log output to ostream.
			inline void InitLog(
				typename LogController::StateToStrFunc     _state_to_str,
				typename LogController::ActionToStrFunc    _action_to_str,
				typename LogController::ResultToStrFunc    _result_to_str
			)
			{
				_log_controller.Init(_state_to_str, _action_to_str, _result_to_str);
			}

			//enable log by ostream.
			inline void EnableLog(std::ostream& os = std::cout)
			{
				_log_controller.EnableLog(os);
			}

			//disable log.
			inline void DisableLog()
			{
				_log_controller.DisableLog();
			}

			//enable json output
			inline void EnableJsonOutput(std::string output_path = "./MctsOutput.json")
			{
				_log_controller.EnableJsonOutput(output_path);
			}

			//disable json output.
			inline void DisableJsonOutput()
			{
				_log_controller.DisableJsonOutput();
			}
		};

		template<typename State, typename Action, typename Result, bool _is_debug = false>
		class LockFreeMonteCarloTreeSearch
		{

		};
	}
}