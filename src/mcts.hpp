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
#include "gadt_algorithm.hpp"

#pragma once

namespace gadt
{
	//monte carlo tree search.
	namespace mcts
	{
		/*
		* MctsSetting is the setting of MCTS.
		*
		* MctsSetting() would use default setting.
		* MctsSetting(params) would generate custom setting.
		*/
		struct MctsSetting final: public GameAlgorithmSettingBase
		{
			size_t thread_num;
			size_t max_iteration_per_thread;	//set max iteration times.
			size_t max_node_per_thread;			//pre-allocated memory for each thread.
			size_t simulation_warning_length;

			//default setting constructor.
			MctsSetting() :
				GameAlgorithmSettingBase(),
				thread_num(1),
				max_iteration_per_thread(10000),
				max_node_per_thread(10000),
				simulation_warning_length(1000)
			{
			}

			//custom setting constructor.
			MctsSetting(
				double _timeout, 
				size_t _thread_num,	
				size_t _max_iteration_per_thread,
				size_t _max_node_per_thread,
				AgentIndex _no_winner_index = 0,
				size_t _simulation_warning_length = 1000
			) :
				GameAlgorithmSettingBase(_timeout,_no_winner_index),
				thread_num(_thread_num),
				max_iteration_per_thread(_max_iteration_per_thread),
				max_node_per_thread(_max_node_per_thread),
				simulation_warning_length(_simulation_warning_length)
			{
			}

			//output print with str behind each line.
			std::string info() const override
			{
				log::ConsoleTable tb(2, 6);
				tb.set_width({ 12,6 });
				tb.enable_title({"MCTS SETTING"});
				tb.set_cell_in_row(0, { { "timeout" },					{ ToString(timeout) } });
				tb.set_cell_in_row(1, { { "thread_num" },				{ ToString(thread_num) } });
				tb.set_cell_in_row(2, { { "max_iteration_per_thread" },	{ ToString(max_iteration_per_thread) } });
				tb.set_cell_in_row(3, { { "max_node_per_thread" },		{ ToString(max_node_per_thread) } });
				tb.set_cell_in_row(4, { { "no_winner_index" },			{ ToString(no_winner_index) } });
				tb.set_cell_in_row(5, { { "simulation_warning_length" },{ ToString(simulation_warning_length) } });
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
			using Node			= MctsNode<State, Action, Result, _is_debug>;			//MctsNode
			using pointer       = Node*;												//pointer of MctsNode
			using reference     = Node&;												//reference of MctsNode
			using Allocator		= gadt::stl::LinearAllocator<Node, _is_debug>;			//Allocate 
			using FuncPackage	= MctsFuncPackage<State, Action, Result, _is_debug>;	//function package
			using ActionList	= typename FuncPackage::ActionList;						//ActionList is the list of available actions.

		private:
			State			_state;				//state of this node.
			AgentIndex		_winner_index;		//the winner index of the state.
			uint32_t		_visited_time;		//how many times that this node had been visited.
			uint32_t		_win_time;			//win time accmulated by the simulation.
			ActionList		_action_list;		//action set of this node.
			
			pointer			_parent_node;		//pointer to parent node
			pointer			_fir_child_node;	//pointer to left most child node.
			pointer			_brother_node;		//pointer to breother node.

		public:
			const State&        state()               const { return _state; }
			const ActionList&   action_list()         const { return _action_list; }
			const Action&       action(size_t i)      const { return _action_list[i]; }
			size_t              action_num()          const { return _action_list.size(); }
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

			//exist unactived action in the action set.
			inline bool exist_unactivated_action() const
			{
				size_t action_size = _action_list.size();
				volatile size_t child_size = child_num();
				if (action_size == 0)
					return false;
				else if (action_size == child_size)
					return false;
				return true;
			}

			//get next action.
			inline const Action& next_action()
			{
				return _action_list[child_num()];
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
				bool b = allocator.destory(this);
				GADT_CHECK_WARNING(is_debug(), b == false, "MCTS105: free child node failed.");
			}

		public:
			MctsNode(const State& state, pointer parent_node, const FuncPackage& func, const MctsSetting& setting) :
				_state(state),
				_winner_index(func.DetemineWinner(state)),
				_visited_time(1),
				_win_time(0),
				_parent_node(parent_node),
				_fir_child_node(nullptr),
				_brother_node(nullptr)
			{
				if (!is_end_state(setting))
				{
					func.MakeAction(_state, _action_list);
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
			Result Simulation(const FuncPackage& func, const MctsSetting& setting)
			{
				State state = _state;	//copy
				ActionList actions;
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
					GADT_CHECK_WARNING(is_debug(), actions.size() == 0, "empty action list.");

					//choose action by default policy.
					const Action& action = func.DefaultPolicy(actions);

					//state update.
					func.UpdateState(state, action);
				}
				return func.StateToResult(_state, setting.no_winner_index);
			}

			//2.one child node would be added to expand the tree, acccording to the available actions.
			void Expandsion(Allocator& allocator ,const FuncPackage& func, const MctsSetting& setting)
			{
				if (is_end_state(setting))
				{
					Result result = func.StateToResult(_state, _winner_index);//return the result of this node.
					BackPropagation(result, func);
					return; //back propagate from this node to root.
				}
				else
				{
					volatile const size_t new_child_index = child_num();
					if (new_child_index < _action_list.size())
					{
						//create new node.
						State new_state = _state;
						func.UpdateState(new_state, _action_list[new_child_index]);
						pointer new_node = allocator.construct(new_state, this, func, setting);
						Result result = new_node->Simulation(func, setting);
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

				if (is_end_state(setting))
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
						GADT_CHECK_WARNING(is_debug(), _action_list.size() == 0, "MCTS106: empty action set during tree policy.");

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
			inline bool is_end_state(const MctsSetting& setting) const
			{
				return _winner_index != setting.no_winner_index;
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
					ss << child_num() << "/" << action_list().size();
				}
				else
				{
					ss << action_list().size() << "/" << action_list().size();
				}
				ss << " }";
				return ss.str();
			}

			//get the size of subtree whose root node is this.
			size_t subtree_size() const
			{
				size_t count = 1;
				pointer child = fir_child_node();
				while (child != nullptr)
				{
					count += child->subtree_size();
					child = child->brother_node();
				}
				return count;
			}

			//return the value of _is_debug.
			constexpr inline bool is_debug() const
			{
				return _is_debug;
			}
		};

		/*
		* MctsFuncPackage include all necessary functions for MCTS.
		*
		* use MctsFuncPackage(...) to construct a new function package.
		*/
		template<typename State, typename Action, typename Result, bool _is_debug>
		struct MctsFuncPackage final : public GameAlgorithmFuncPackageBase<State, Action, _is_debug>
		{
		public:
#ifdef __GADT_GNUC
			using ActionList = typename GameAlgorithmFuncPackageBase<State, Action, _is_debug>::ActionList;
			using UpdateStateFunc = typename GameAlgorithmFuncPackageBase<State, Action, _is_debug>::UpdateStateFunc;
			using MakeActionFunc = typename GameAlgorithmFuncPackageBase<State, Action, _is_debug>::MakeActionFunc;
			using DetemineWinnerFunc = typename GameAlgorithmFuncPackageBase<State, Action, _is_debug>::DetemineWinnerFunc;
			using GameAlgorithmFuncPackageBase<State, Action, _is_debug>::is_debug;
#endif
			using Node					= MctsNode<State, Action, Result, _is_debug>;
			using StateToResultFunc		= std::function<Result(const State&, AgentIndex)>;
			using AllowUpdateValueFunc	= std::function<bool(const State&, const Result&)>;
			using TreePolicyValueFunc	= std::function<UcbValue(const Node&, const Node&)>;
			using DefaultPolicyFunc		= std::function<const Action&(const ActionList&)>;
			using AllowExtendFunc		= std::function<bool(const Node&)>;
			using ValueForRootNodeFunc	= std::function<UcbValue(const Node&, const Node&)>;

		public:
			//necessary functions.
			const StateToResultFunc		StateToResult;		//get a result from state and winner.
			const AllowUpdateValueFunc	AllowUpdateValue;	//update values in the node by the result.

			//default functions.
			TreePolicyValueFunc			TreePolicyValue;	//value of child node in selection process. the highest would be seleced.
			DefaultPolicyFunc			DefaultPolicy;		//the default policy to select action.
			AllowExtendFunc				AllowExtend;		//allow node to extend child node.
			ValueForRootNodeFunc		ValueForRootNode;	//select best action of root node after iterations finished.

		public:
			explicit MctsFuncPackage(
				UpdateStateFunc			_UpdateState,
				MakeActionFunc			_MakeAction,
				DetemineWinnerFunc		_DetemineWinner,
				StateToResultFunc		_StateToResult,
				AllowUpdateValueFunc	_AllowUpdateValue,
				TreePolicyValueFunc		_TreePolicyValue,
				DefaultPolicyFunc		_DefaultPolicy,
				AllowExtendFunc			_AllowExtend,
				ValueForRootNodeFunc	_ValueForRootNode
			) :
				GameAlgorithmFuncPackageBase<State, Action, _is_debug>(_UpdateState, _MakeAction, _DetemineWinner),
				StateToResult(_StateToResult),
				AllowUpdateValue(_AllowUpdateValue),
				TreePolicyValue(_TreePolicyValue),
				DefaultPolicy(_DefaultPolicy),
				AllowExtend(_AllowExtend),
				ValueForRootNode(_ValueForRootNode)
			{
			}

			explicit MctsFuncPackage(
				UpdateStateFunc			_UpdateState,
				MakeActionFunc			_MakeAction,
				DetemineWinnerFunc		_DetemineWinner,
				StateToResultFunc		_StateToResult,
				AllowUpdateValueFunc	_AllowUpdateValue
			) :
				GameAlgorithmFuncPackageBase<State, Action, _is_debug>(_UpdateState, _MakeAction, _DetemineWinner),
				StateToResult(_StateToResult),
				AllowUpdateValue(_AllowUpdateValue),
				TreePolicyValue([](const Node& parent, const Node& child)->UcbValue {
					UcbValue avg = static_cast<UcbValue>(child.win_time()) / static_cast<UcbValue>(child.visited_time());
					return policy::UCB1(avg, static_cast<UcbValue>(parent.visited_time()), static_cast<UcbValue>(child.visited_time()));
				}),
				DefaultPolicy([](const ActionList& actions)->const Action&{
					GADT_CHECK_WARNING(_is_debug, actions.size() == 0, "MCTS104: empty action set during default policy.");
					return actions[rand() % actions.size()];
				}),
				AllowExtend([](const Node& node)->bool {
					return true;
				}),
				ValueForRootNode([](const Node& parent, const Node& child)->UcbValue {
					return static_cast<UcbValue>(child.visited_time());
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
		private:
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
				//visual_node.add_value(IS_TERMIANL_NAME, search_node.is_end_state());
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
		* [_no_winner_index] means the state do not have winner.
		* [_is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename State, typename Action, typename Result = AgentIndex, bool _is_debug = false>
		class MonteCarloTreeSearch final : public GameAlgorithmBase<State,Action,Result,_is_debug>
		{
#ifdef __GADT_GNUC
		private:
			using GameAlgorithmBase<State, Action, Result, _is_debug>::_algorithm_name;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::_log_controller;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::logger;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::log_enabled;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::json_output_enabled;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::timeout;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::is_debug;

		public:
			using GameAlgorithmBase<State, Action, Result, _is_debug>::SetName;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::InitLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::EnableLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::DisableLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::EnableJsonOutput;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::DisableJsonOutput;
#endif

		public:
			using Node			= MctsNode<State, Action, Result, _is_debug>;			//searcg node.	
			
		private:
			using Allocator     = typename Node::Allocator;								//allocator of nodes
			using LogController = log::SearchLogger<State, Action, Result>;				//log controller
			using JsonConvert   = MctsJsonConvertor<State, Action, Result, _is_debug>;	//json tree
			using FuncPackage   = MctsFuncPackage<State, Action, Result, _is_debug>;	//function package.
			using ActionList	= typename FuncPackage::ActionList;						//set of Action

		private:
			FuncPackage		_func_package;			//function package of the search.
			MctsSetting		_setting;				//monte carlo tree search setting.

		private:

			//excute iteration function.
			Action ExcuteMCTS(State root_state)
			{
				timer::TimePoint tp_mcts_start;

				//outputt log.
				if (log_enabled())
				{
					logger() << "[ Monte Carlo Tree Search ]" << std::endl;
					logger() << _setting.info();
					logger() << std::endl << ">> Executing MCTS......" << std::endl;
				}

				//initialized
				Node root_node(root_state, nullptr, _func_package, _setting);
				ActionList root_actions = root_node.action_list();
				
				//thread content
				stl::LinearAllocator<Allocator, _is_debug> allocators(_setting.thread_num);
				std::vector<std::thread> threads;

				timer::TimePoint tp_thread_start;
				//enable multithread.
				for (size_t thread_id = 0; thread_id < _setting.thread_num; thread_id++)
				{
					Allocator* thread_allocator = allocators.construct(_setting.max_node_per_thread);
					threads.push_back(std::thread([&](Allocator* allocator)->void {
						size_t iteration_time = _setting.max_iteration_per_thread;
						for (size_t i = 0; i < iteration_time; i++)
						{
							//stop search if timout.
							if (this->timeout(tp_mcts_start, _setting)) { return; }

							//excute garbage collection if need.
							if (allocator->is_full())
							{
								break;//run out of memory, stop search.
							}

							//excute next.
							root_node.Selection(*allocator, _func_package, _setting);
						}
					}, thread_allocator));
				}

				//join all threads.
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
				GADT_CHECK_WARNING(is_debug(), root_node.fir_child_node() == nullptr, "MCTS107: empty child node under root node.");
				Node* child_ptr = root_node.fir_child_node();
				std::vector<Node*> child_ptr_set;
				std::vector<UcbValue> child_value_set;
				while(child_ptr != nullptr)
				{
					child_ptr_set.push_back(child_ptr);
					child_value_set.push_back(_func_package.ValueForRootNode(root_node, *child_ptr));
					child_ptr = child_ptr->brother_node();
				}
				size_t best_node_index = func::GetMaxElement(child_value_set);

				if (log_enabled())
				{
					std::vector<size_t> tree_size_set(root_actions.size());
					std::vector<size_t> visit_time_set(root_actions.size());
					std::vector<size_t> win_time_set(root_actions.size());
					size_t total_tree_size = 0;
					size_t total_visit_time = 0;
					size_t total_win_time = 0;
					for (size_t i = 0; i < child_ptr_set.size(); i++)
					{
						tree_size_set[i] = child_ptr_set[i]->subtree_size();
						visit_time_set[i] = child_ptr_set[i]->visited_time();
						win_time_set[i] = child_ptr_set[i]->win_time();
						total_tree_size += tree_size_set[i];
						total_visit_time += visit_time_set[i];
						total_win_time += win_time_set[i];
					}

					//MCTS RESULT
					log::ConsoleTable tb(7, root_node.action_list().size() + 2);
					tb.enable_title({ "MCTS RESULT: TIME = [ " + ToString(tp_mcts_start.time_since_created()) + "s ]" });
					tb.set_cell_in_row(0, { { "Index" },{ "Action" },{ "Value" },{ "Visit" },{ "Win" },{ "Size" },{ "Best" } });
					tb.set_width({ 3,10,4,4,4,4,2 });
					for (size_t i = 0; i < root_node.action_list().size(); i++)
					{
						tb.set_cell_in_row(i + 1, {
							{ ToString(i) },
							{ _log_controller.action_to_str_func()(root_node.action_list()[i]) },
							{ ToString(child_value_set[i]) },
							{ ToString(visit_time_set[i])},
							{ ToString(win_time_set[i]) },
							{ ToString(tree_size_set[i]) },
							{ i == best_node_index ? "Yes " : "  " }
						});
					}
					tb.set_cell_in_row(root_node.action_list().size()+1, { 
						{ "Total" },
						{ "" },
						{ "" },
						{ ToString(total_visit_time) },
						{ ToString(total_win_time) },
						{ ToString(total_tree_size) },
						{ ToString(best_node_index) } 
					});
					logger() << tb.output_string() << std::endl;
				}

				GADT_CHECK_WARNING(is_debug(), root_actions.size() == 0, "MCTS102: best value for root node equal to 0.");

				return root_actions[best_node_index];
			}

		public:
			//use private allocator.
			MonteCarloTreeSearch(
				typename FuncPackage::UpdateStateFunc		_UpdateState,
				typename FuncPackage::MakeActionFunc		_MakeAction,
				typename FuncPackage::DetemineWinnerFunc	_DetemineWinner,
				typename FuncPackage::StateToResultFunc		_StateToResult,
				typename FuncPackage::AllowUpdateValueFunc	_AllowUpdateValue
			):
				GameAlgorithmBase<State, Action, Result, _is_debug>("MCTS"),
				_func_package( _UpdateState, _MakeAction, _DetemineWinner, _StateToResult, _AllowUpdateValue),
				_setting()
			{
			}


			//use private allocator.
			MonteCarloTreeSearch(FuncPackage function_package, size_t max_node) :
				GameAlgorithmBase<State, Action, Result, _is_debug>("MCTS"),
				_func_package(function_package),
				_setting()
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
		};
	}

	//old implement of the monte carlo tree search. it may be deleted someday.
	namespace mcts_old
	{
		//allow check warning if it is true.
		constexpr const bool g_MCTS_ENABLE_WARNING = true;

		//State Info
		template <
			typename t_State,
			typename t_Action,
			void(*make_action_func)(const t_State&, std::vector<t_Action>&),
			int(*winner_determind_func)(const t_State&),
			bool debug_model
		>
			class StateInfo final
		{
		private:
			t_State _state;
			std::vector<t_Action> _action_vec;
			size_t _next_action_index;
			int _winner;

		public:
			StateInfo(const t_State& state) :
				_state(state),
				_next_action_index(0),
				_winner(winner_determind_func(state))
			{
				if (_winner < 0)//no winner.
				{
					make_action_func(state, _action_vec);
					if (debug_model)
					{
						GADT_CHECK_WARNING(g_MCTS_ENABLE_WARNING, _action_vec.size() == 0, "wrong make action func");
					}
				}
			}

			inline const t_Action& next_action()
			{
				return _action_vec[_next_action_index++];
			}

			inline const t_Action& random_action() const
			{
				return _action_vec[rand() % _action_vec.size()];
			}

			inline const t_Action& action(size_t index) const
			{
				return _action_vec[index];
			}

			inline bool exist_unactivated_action() const
			{
				return _next_action_index != _action_vec.size();
			}

			inline int winner() const
			{
				return _winner;
			}

			inline const t_State& state() const
			{
				return _state;
			}

			inline bool is_end_state() const
			{
				return _winner >= 0;
			}
		};

		//MCTS result.
		template <
			typename t_State
		>
			class MctsResult final
		{
		private:
			t_State _result_state;
			int _result_winner;

		public:
			MctsResult(t_State state, int winner) :
				_result_state(state),
				_result_winner(winner)
			{
			}

			const t_State& state()
			{
				return _result_state;
			}

			int winner()
			{
				return _result_winner;
			}
		};


		//MCTS node.
		template <
			typename t_State,
			typename t_Action,
			void(*state_generate_func)(t_State&, int&, const t_Action&),
			void(*make_action_func)(const t_State&, std::vector<t_Action>&),
			int(*winner_determind_func)(const t_State&),
			bool debug_model
		>
			class MctsNode final
		{
			typedef StateInfo<t_State, t_Action, make_action_func, winner_determind_func, debug_model> t_StateInfo;
			typedef MctsResult<t_State> t_MctsResult;

			//Custom Function.
			typedef std::function<void(t_State&, int&, const t_Action&)> FuncStateGenerate;
			typedef std::function<void(const t_State&, std::vector<t_Action>&)> FuncMakeAction;
			typedef std::function<int(const t_State&, int)> FuncWinnerDetermind;

			typedef std::function<void(const t_State&, int, size_t&, int)> FuncBackpropagate;
			typedef std::function<double(size_t, size_t, size_t, size_t)> FuncUcbForParent;
			typedef std::function<void(const t_State&, int, t_State&, int&)> FuncDefaultPolicy;
			typedef std::function<bool(const t_State&, size_t, size_t)> FuncNodeExtend;

		private:
			t_StateInfo* _state_info;
			size_t _visited_time;
			size_t _win_time;
			int _player_index;
			std::vector<MctsNode*> _child_node;

		private:

			//clear state info of this node. NOTICE: this function should be executed after all child node of this node had been created.
			inline void clear_state_info()
			{
				delete _state_info;
				_state_info = nullptr;
			}

			//get whether the state info exist or had been cleard.
			inline bool exist_state_info()
			{
				return _state_info != nullptr;
			}

			//default policy. return a temp object to 
			const t_MctsResult* DefaultPolicy(FuncDefaultPolicy default_policy_func)
			{
				t_State result_state = _state_info->state();
				int result_winner = _state_info->winner();
				default_policy_func(_state_info->state(), result_state, result_winner);
				return new t_MctsResult(result_state, result_winner);
			}

		public:
			//create root node.
			MctsNode(t_State state, int player_index) :
				_state_info(new t_StateInfo(state)),
				_player_index(player_index),
				_visited_time(1),
				_win_time(0)
			{
			}

			//copy constructor function.
			MctsNode(const MctsNode& target) :
				_state_info(nullptr),
				_visited_time(target._visited_time),
				_win_time(target._win_time),
				_player_index(target._player_index)
			{
				if (target._state_info != nullptr)
				{
					_state_info = new t_StateInfo(*target._state_info);
				}
			}

			~MctsNode()
			{
				if (_state_info != nullptr)
				{
					delete _state_info;
				}
				if (_child_node.size() != 0)
				{
					for (auto child : _child_node)
					{
						delete child;
					}
				}
			}

			//get the best child index that have best ucb value.
			size_t GetBestChild(FuncUcbForParent ucb_for_parent_func)
			{
				double best_ucb = 0;
				size_t best_child = 0;
				for (size_t i = 0; i < _child_node.size(); i++)
				{
					double ucb = ucb_for_parent_func(_child_node[i]->_visited_time, _child_node[i]->_win_time, this->_visited_time, this->_win_time);
					if (ucb > best_ucb)
					{
						best_ucb = ucb;
						best_child = i;
					}
				}
				return best_child;
			}

			//extend node. do action according to the state of node.
			t_MctsResult* Extend(
				FuncBackpropagate back_propagate_func,
				FuncUcbForParent ucb_for_parent_func,
				FuncDefaultPolicy default_policy_func,
				FuncNodeExtend node_extend_func,
				bool auto_clear_state_info
			)
			{
				//increase the visited time of this node.
				_visited_time++;

				//check winner -> check unactivated action -> add new node.
				if (_state_info != nullptr)
				{
					if (_state_info->is_end_state())
					{
						//return current state.
						return new t_MctsResult(_state_info->state(), _state_info->winner());
					}

					if (_state_info->exist_unactivated_action())
					{
						if (_child_node.size() == 0)
						{
							if (!node_extend_func(_state_info->state(), _visited_time, _win_time))
							{
								//do not extend, return default policy of current node;
								t_State result_state = _state_info->state();
								int result_winner = 0;
								default_policy_func(_state_info->state(), this->_player_index, result_state, result_winner);
								back_propagate_func(result_state, result_winner, this->_win_time, this->_player_index);
								return new t_MctsResult(result_state, result_winner);
							}
						}
						//generate new child.
						t_State child_state = this->_state_info->state();
						int child_player = this->_player_index;
						state_generate_func(child_state, child_player, this->_state_info->next_action());
						MctsNode* child = new MctsNode(child_state, child_player);
						_child_node.push_back(child);

						if (auto_clear_state_info)
						{
							if (this->state_info()->exist_unactivated_action() == false)
							{
								this->clear_state_info();
							}
						}

						if (child->state_info()->is_end_state())
						{
							back_propagate_func(child->state_info()->state(), child->state_info()->winner(), child->_win_time, child->player_index());
							back_propagate_func(child->state_info()->state(), child->state_info()->winner(), this->_win_time, this->player_index());
							return new t_MctsResult(child->state_info()->state(), child->state_info()->winner());
						}
						else
						{
							//do default policy in child node.
							t_State result_state = child->state_info()->state();
							int result_winner = 0;
							default_policy_func(child->state_info()->state(), child->player_index(), result_state, result_winner);
							back_propagate_func(result_state, result_winner, child->_win_time, child->player_index());
							back_propagate_func(result_state, result_winner, this->_win_time, this->player_index());
							return new t_MctsResult(result_state, result_winner);
						}
					}
				}

				//do not return. find the child with best ucb value;
				size_t best_child = GetBestChild(ucb_for_parent_func);
				t_MctsResult* result = this->_child_node[best_child]->Extend(
					back_propagate_func,
					ucb_for_parent_func,
					default_policy_func,
					node_extend_func,
					auto_clear_state_info
				);
				back_propagate_func(result->state(), result->winner(), this->_win_time, this->player_index());
				return result;
			}

			size_t TreeSizeAsRoot()
			{
				size_t s = 1;//node itself
				for (auto child : child_node_ref())
				{
					s += child->TreeSizeAsRoot();
				}
				return s;
			}

			//get the state info as a const format.
			inline const t_StateInfo* const state_info() const
			{
				return _state_info;
			}
			inline int player_index() const
			{
				return _player_index;
			}
			inline size_t visited_time() const
			{
				return _visited_time;
			}
			inline size_t win_time() const
			{
				return _win_time;
			}
			inline void add_win_time(size_t t)
			{
				_win_time += t;
			}
			inline double exception()
			{
				return (double)_win_time / (double)_visited_time;
			}
			inline std::vector<MctsNode*> child_node_ref()
			{
				return _child_node;
			}
		};

		//MCTS Search.
		template <
			typename t_State,
			typename t_Action,
			void(*state_generate_func)(t_State&, int&, const t_Action&),
			void(*make_action_func)(const t_State&, std::vector<t_Action>&),
			int(*winner_determind_func)(const t_State&),
			bool debug_model = false
		>
			class MctsSearch final
		{
			//Members
			typedef StateInfo<t_State, t_Action, make_action_func, winner_determind_func, debug_model> t_StateInfo;
			typedef MctsNode<t_State, t_Action, state_generate_func, make_action_func, winner_determind_func, debug_model> t_MctsNode;
			typedef MctsResult<t_State> t_MctsResult;

			//Custom Function.
			typedef std::function<void(t_State&, int&, const t_Action&)> FuncStateGenerate;
			typedef std::function<void(const t_State&, std::vector<t_Action>&)> FuncMakeAction;
			typedef std::function<int(const t_State&, int)> FuncWinnerDetermind;

			typedef std::function<void(const t_State&, int, size_t&, int)> FuncBackpropagate;
			typedef std::function<double(size_t, size_t, size_t, size_t)> FuncUcbForParent;
			typedef std::function<void(const t_State&, int, t_State&, int&)> FuncDefaultPolicy;
			typedef std::function<bool(const t_State&, size_t, size_t)> FuncNodeExtend;
			typedef std::function<void(t_State&)> FuncShowState;
			typedef std::function<void(t_Action&)> FuncShowAction;

		private:
			//custom function.
			FuncBackpropagate _func_back_propagate;
			FuncUcbForParent _func_ucb_for_parent;
			FuncDefaultPolicy _func_default_policy;
			FuncNodeExtend _func_node_extend;

			//setting
			bool _auto_clear_state_info;			//default is true.

		private:
			void FuncInit()
			{
				_func_back_propagate = [](const t_State& result_state, int result_winner, size_t& node_win_time, int node_player)->void
				{
					if (result_winner == node_player)
					{
						node_win_time++;
					}
				};

				_func_ucb_for_parent = [](size_t child_visited_time, size_t child_win_time, size_t parent_visited_time, size_t parent_win_time)->double
				{
					double avg = 1 - ((double)child_win_time / (double)child_visited_time);
					double bound = sqrt(2 * log10(static_cast<double>(parent_visited_time)) / static_cast<double>(child_visited_time));
					return avg + bound;
				};

				_func_default_policy = [](const t_State& original_state, int origianl_player, t_State& result_state, int& result_winner)->void
				{
					t_State temp_state = original_state;
					int temp_player = origianl_player;
					if (debug_model)
					{
						//default policy should not be execute in a end state.
						t_StateInfo original_state_info(temp_state);
						GADT_CHECK_WARNING(g_MCTS_ENABLE_WARNING, original_state_info.is_end_state(), "do default policy in a end state.");
					}

					for (;;)
					{
						t_StateInfo state_info(temp_state);

						//reach a leaf node.
						if (state_info.is_end_state())
						{
							result_state = temp_state;
							result_winner = state_info.winner();
							break;
						}

						//generate next state.
						state_generate_func(temp_state, temp_player, state_info.random_action());
					}
				};

				_func_node_extend = [](const t_State& node_state, size_t node_visited_time, size_t node_win_time)->bool
				{
					return true;
				};

			}

		public:
			inline void SetBackPropagateFunc(FuncBackpropagate func)
			{
				_func_back_propagate = func;
			}
			inline void SetUcbForParentFunc(FuncUcbForParent func)
			{
				_func_ucb_for_parent = func;
			}
			inline void SetDefaultPolicyFunc(FuncDefaultPolicy func)
			{
				_func_default_policy = func;
			}
			inline void SetNodeExtendFunc(FuncNodeExtend func)
			{
				_func_node_extend = func;
			}

			//clear state info in the node after all child nodes of it have been generated.
			inline void SetAutoClearStateInfo(bool b)
			{
				_auto_clear_state_info = b;
			}
		public:

			//Mcts Search.
			MctsSearch() :
				_auto_clear_state_info(true)
			{
				FuncInit();
			}

			//Do MCTS.
			t_Action DoMCTS(t_State original_state, int original_player, size_t iteration_time)
			{
				clock_t f_start = clock();//set timer
				t_MctsNode* root_node = new t_MctsNode(original_state, original_player);
				t_StateInfo root_info = *root_node->state_info();//copy
				if (debug_model)
				{
					GADT_CHECK_WARNING(g_MCTS_ENABLE_WARNING, root_info.is_end_state(), "start from leaf node.");
				}
				//iteration.
				for (size_t i = 0; i < iteration_time; i++)
				{
					t_MctsResult* result = root_node->Extend(
						_func_back_propagate,
						_func_ucb_for_parent,
						_func_default_policy,
						_func_node_extend,
						_auto_clear_state_info
					);
					delete result;
				}
				size_t best_child = root_node->GetBestChild(_func_ucb_for_parent);
				if (debug_model)
				{
					std::cout << "MCTS DEBUG INFO ->" << std::endl << std::endl;
					std::cout << ">> iteration = " << iteration_time << std::endl;
					std::cout << ">> tree size = " << root_node->TreeSizeAsRoot() << std::endl;
					std::cout << ">> time consume = " << (double)(clock() - f_start) / CLOCKS_PER_SEC << " s." << std::endl;
					std::cout << ">> result is action " << best_child << std::endl << std::endl;

					for (size_t i = 0; i < root_node->child_node_ref().size(); i++)
					{
						std::cout << ">> node " << i << ": payoff = " << root_node->child_node_ref()[i]->exception() << " ( ";
						std::cout << root_node->child_node_ref()[i]->win_time() << "/" << root_node->child_node_ref()[i]->visited_time();
						std::cout << " ) for player " << root_node->child_node_ref()[i]->player_index() << std::endl;
					}

					std::cout << std::endl;
					std::cout << ">> node exception = " << root_node->exception() << std::endl << std::endl;
				}

				delete root_node;
				return root_info.action(best_child);
			}
		};
	}
}