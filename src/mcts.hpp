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
#include "gadt_table.h"

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
			size_t max_thread;
			size_t max_iteration_per_thread;	//set max iteration times.
			size_t max_node_per_thread;			//pre-allocated memory for each thread.
			size_t simulation_warning_length;

			//default setting constructor.
			MctsSetting() :
				GameAlgorithmSettingBase(),
				max_thread(1),
				max_iteration_per_thread(10000),
				max_node_per_thread(10000),
				simulation_warning_length(1000)
			{
			}

			//custom setting constructor.
			MctsSetting(
				double _timeout, 
				size_t _max_thread,	
				size_t _max_iteration_per_thread,
				size_t _max_node_per_thread,
				AgentIndex _no_winner_index = 0,
				size_t _simulation_warning_length = 1000
			) :
				GameAlgorithmSettingBase(_timeout,_no_winner_index),
				max_thread(_max_thread),
				max_iteration_per_thread(_max_iteration_per_thread),
				max_node_per_thread(_max_node_per_thread),
				simulation_warning_length(_simulation_warning_length)
			{
			}

			//output print with str behind each line.
			void PrintInfo() const override
			{
				console::Table tb(2, 6);
				tb.set_width({ 12,6 });
				tb.enable_title({"MCTS Setting", console::TABLE_ALIGN_MIDDLE });
				tb.set_cell_in_row(0, { { "timeout" },					{ ToString(timeout) } });
				tb.set_cell_in_row(1, { { "max_thread" },				{ ToString(max_thread) } });
				tb.set_cell_in_row(2, { { "max_iteration_per_thread" },	{ ToString(max_iteration_per_thread) } });
				tb.set_cell_in_row(3, { { "max_node_per_thread" },		{ ToString(max_node_per_thread) } });
				tb.set_cell_in_row(4, { { "no_winner_index" },			{ ToString(no_winner_index) } });
				tb.set_cell_in_row(5, { { "simulation_warning_length" },{ ToString(simulation_warning_length) } });
				tb.Print();
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
			uint32_t		_visit_count;		//how many times that this node had been visited.
			uint32_t		_win_count;			//win time accmulated by the simulation.
			ActionList		_action_list;		//action set of this node.
			
			pointer			_parent_node;		//pointer to parent node
			pointer			_fir_child_node;	//pointer to left most child node.
			pointer			_brother_node;		//pointer to breother node.

		public:

			const State&		state()					const { return _state; }
			const ActionList&	action_list()			const { return _action_list; }
			const Action&		action(size_t i)		const { return _action_list[i]; }
			size_t				action_num()			const { return _action_list.size(); }
			AgentIndex			winner_index()			const { return _winner_index; }
			uint32_t			visit_count()			const { return _visit_count; }
			uint32_t			win_count()				const { return _win_count; }
			double				avg_reward()			const { return static_cast<double>(win_count()) / static_cast<double>(visit_count()); }

			pointer				parent_node()			const { return _parent_node; }
			pointer				fir_child_node()		const { return _fir_child_node; }
			pointer				brother_node()			const { return _brother_node; }

			bool				exist_parent_node()		const { return _parent_node != nullptr; }
			bool				exist_child_node()		const { return _fir_child_node != nullptr; }
			bool				exist_brother_node()	const { return _brother_node != nullptr; }
			bool				is_root()				const { return _parent_node == nullptr; }
			
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
			inline void incr_visit_count()
			{
				_visit_count++;
			}

			//increase win time.
			inline void incr_win_count()
			{
				_win_count++;
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
				_winner_index(func.DetermineWinner(state)),
				_visit_count(1),
				_win_count(0),
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

			//4.the simulation result is back propagated through the selected nodes to update their statistics.
			void BackPropagation(const Result& result, const FuncPackage& func)
			{
				if (exist_parent_node())
				{
					//update node if allow.
					if (func.AllowUpdateValue(parent_node()->state(), result))
					{
						incr_win_count();
					}

					//update parent node.
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
					AgentIndex winner = func.DetermineWinner(state);

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
				incr_visit_count();

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
				double avg = static_cast<double>(win_count()) / static_cast<double>(visit_count());
				ss << "{ visited:" << visit_count() << " win:" << win_count() <<" avg:" << avg << " child";
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

			//get the winning exception of this node.
			double exception() const
			{
				size_t visit_count = 0;
				size_t win_count = 0;
				for (pointer child = fir_child_node(); child != nullptr; child = child->brother_node())
				{
					visit_count += child->visit_count();
					win_count += child->win_count();
				}
				return static_cast<double>(win_count) / static_cast<double>(visit_count);
			}

			//get list of all childs
			std::vector<Node*> child_ptr_set() const
			{
				std::vector<Node*> child_list;
				pointer child = fir_child_node();
				for (;;)
				{
					if (child != nullptr)
					{
						child_list.push_back(child);
						child = child->brother_node();
					}
					else
					{
						return child_list;
					}
				}
				return child_list;
			}

			//set visted count of this node.
			void set_visit_count(uint32_t value)
			{
				_visit_count = value;
			}

			//set wion count of this node.
			void set_win_count(uint32_t value)
			{
				_win_count = value;
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
			using DetermineWinnerFunc = typename GameAlgorithmFuncPackageBase<State, Action, _is_debug>::DetermineWinnerFunc;
			using GameAlgorithmFuncPackageBase<State, Action, _is_debug>::is_debug;
#else
			using GameAlgorithmFuncPackageBase<State, Action, _is_debug>::ActionList;
			using GameAlgorithmFuncPackageBase<State, Action, _is_debug>::UpdateStateFunc;
			using GameAlgorithmFuncPackageBase<State, Action, _is_debug>::MakeActionFunc;
			using GameAlgorithmFuncPackageBase<State, Action, _is_debug>::DetermineWinnerFunc;
#endif
			using Node					= MctsNode<State, Action, Result, _is_debug>;
			using NodePtrList			= std::vector<Node*>;
			using StateToResultFunc		= std::function<Result(const State&, AgentIndex)>;
			using AllowUpdateValueFunc	= std::function<bool(const State&, const Result&)>;
			using TreePolicyValueFunc	= std::function<UcbValue(const Node&, const Node&)>;
			using DefaultPolicyFunc		= std::function<const Action&(const ActionList&)>;
			using AllowExtendFunc		= std::function<bool(const Node&)>;
			using RootSelectionFunc		= std::function<size_t(const Node& root)>;

		public:
			//necessary functions.
			const StateToResultFunc		StateToResult;		//get a result from state and winner.
			const AllowUpdateValueFunc	AllowUpdateValue;	//update values in the node by the result.

			//default functions.
			TreePolicyValueFunc			TreePolicyValue;	//value of child node in selection process. the highest would be seleced.
			DefaultPolicyFunc			DefaultPolicy;		//the default policy to select action.
			AllowExtendFunc				AllowExtend;		//allow node to extend child node.
			RootSelectionFunc			RootSelection;		//select best action of root node after iterations finished.

		public:
			explicit MctsFuncPackage(
				UpdateStateFunc			_UpdateState,
				MakeActionFunc			_MakeAction,
				DetermineWinnerFunc		_DetermineWinner,
				StateToResultFunc		_StateToResult,
				AllowUpdateValueFunc	_AllowUpdateValue,
				TreePolicyValueFunc		_TreePolicyValue,
				DefaultPolicyFunc		_DefaultPolicy,
				AllowExtendFunc			_AllowExtend,
				RootSelectionFunc		_RootSelection
			) :
				GameAlgorithmFuncPackageBase<State, Action, _is_debug>(_UpdateState, _MakeAction, _DetermineWinner),
				StateToResult(_StateToResult),
				AllowUpdateValue(_AllowUpdateValue),
				TreePolicyValue(_TreePolicyValue),
				DefaultPolicy(_DefaultPolicy),
				AllowExtend(_AllowExtend),
				RootSelection(_RootSelection)
			{
			}

			explicit MctsFuncPackage(
				UpdateStateFunc			_UpdateState,
				MakeActionFunc			_MakeAction,
				DetermineWinnerFunc		_DetermineWinner,
				StateToResultFunc		_StateToResult,
				AllowUpdateValueFunc	_AllowUpdateValue
			) :
				GameAlgorithmFuncPackageBase<State, Action, _is_debug>(_UpdateState, _MakeAction, _DetermineWinner),
				StateToResult(_StateToResult),
				AllowUpdateValue(_AllowUpdateValue),
				TreePolicyValue([](const Node& parent, const Node& child)->UcbValue {
					UcbValue avg = static_cast<UcbValue>(child.win_count()) / static_cast<UcbValue>(child.visit_count());
					return policy::UCB1(avg, static_cast<UcbValue>(parent.visit_count()), static_cast<UcbValue>(child.visit_count()));
				}),
				DefaultPolicy([](const ActionList& actions)->const Action&{
					GADT_CHECK_WARNING(_is_debug, actions.size() == 0, "MCTS104: empty action set during default policy.");
					return actions[rand() % actions.size()];
				}),
				AllowExtend([](const Node& node)->bool {
					return true;
				}),
				RootSelection([](const Node& root)->size_t{
					auto node_ptr_set = root.child_ptr_set();
					size_t best_visit = node_ptr_set[0]->visit_count();
					size_t best_index = 0;
					for (size_t i = 0; i < node_ptr_set.size(); i++)
					{
						if (node_ptr_set[i]->visit_count() > best_visit)
						{
							best_visit = node_ptr_set[i]->visit_count();
							best_index = i;
						}
					}
					return best_index;
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
			const char* visit_count_NAME    = "visit_count";
			const char* win_count_NAME        = "win_count";
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
				visual_node.add_value(visit_count_NAME, search_node.visit_count());
				visual_node.add_value(win_count_NAME, search_node.win_count());
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
		* class MctsBase is the base class template of various kind of MCTS.
		*
		* [State] is the game-state class, which is defined by the user.
		* [Action] is the game-action class, which is defined by the user.
		* [Result] is the game-result class, which stand for a terminal state of the game.
		* [_is_debug] decides whether debug info would be ignored or not. which may cause slight degradation in performance if it is enabled.
		*/
		template<typename State, typename Action, typename Result = AgentIndex, bool _is_debug = false>
		class MctsBase : public GameAlgorithmBase<State,Action,Result,_is_debug>
		{
//#ifdef __GADT_GNUC
		private:
			using GameAlgorithmBase<State, Action, Result, _is_debug>::_log_controller;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::logger;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::log_enabled;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::json_output_enabled;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::timeout;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::is_debug;

		public:
			using GameAlgorithmBase<State, Action, Result, _is_debug>::name;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::set_name;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::InitLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::EnableLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::DisableLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::EnableJsonOutput;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::DisableJsonOutput;
//#endif

		private:

			using Node			= MctsNode<State, Action, Result, _is_debug>;			//searcg node.	
			using Allocator     = typename Node::Allocator;								//allocator of nodes
			using LogController = log::SearchLogger<State, Action, Result>;				//log controller
			using JsonConvert   = MctsJsonConvertor<State, Action, Result, _is_debug>;	//json tree
			using FuncPackage   = MctsFuncPackage<State, Action, Result, _is_debug>;	//function package.
			using ActionList	= typename FuncPackage::ActionList;						//set of Action

		protected:

			FuncPackage		_func_package;			//function package of the search.
			MctsSetting		_setting;				//monte carlo tree search setting.

		protected:

			//print result of MCTS
			void PrintResult(Node& root_node, size_t best_child_index, const timer::TimePoint& tp) const
			{
				auto child_nodes = root_node.child_ptr_set();
				std::vector<size_t> tree_size_set(root_node.action_num());
				size_t total_tree_size = 0;
				size_t total_win_count = 0;
				for (size_t i = 0; i < child_nodes.size(); i++)
				{
					tree_size_set[i] = child_nodes[i]->subtree_size();
					total_tree_size += tree_size_set[i];
					total_win_count += child_nodes[i]->win_count();
				}

				//MCTS RESULT
				console::Table tb(7, root_node.action_num() + 2);
				tb.enable_title({ 
					name() + " Result: Time = [ " + ToString(tp.time_since_created()) + "s ]", 
					console::COLOR_GRAY, 
					console::TABLE_ALIGN_MIDDLE
					});
				tb.set_cell_in_row(0, {
					{ "Index", console::COLOR_GRAY, console::TABLE_ALIGN_MIDDLE },
					{ "Size", console::COLOR_GRAY },
					{ "Visit", console::COLOR_GRAY },
					{ "Win", console::COLOR_GRAY },
					{ "Avg Reward", console::COLOR_GRAY },
					{ "Best", console::COLOR_GRAY },
					{ "Action", console::COLOR_GRAY, console::TABLE_ALIGN_MIDDLE }
					});
				tb.set_width({ 4,4,4,4,5,2,25 });
				for (size_t i = 0; i < root_node.action_num(); i++)
				{
					tb.set_cell_in_row(i + 1, {
						{ i, console::COLOR_GRAY, console::TABLE_ALIGN_MIDDLE },
						{ tree_size_set[i] },
						{ child_nodes[i]->visit_count()},
						{ child_nodes[i]->win_count() },
						{ child_nodes[i]->avg_reward() },
						{ i == best_child_index ? " Yes" : "" },
						{ _log_controller.action_to_str_func()(root_node.action(i)), console::TABLE_ALIGN_MIDDLE }
						});
				}
				tb.set_cell_in_row(root_node.action_num() + 1, {
					{ "Total", console::COLOR_GRAY, console::TABLE_ALIGN_MIDDLE },
					{ total_tree_size },
					{ root_node.visit_count() },
					{ total_win_count },
					{ root_node.exception() },
					{ best_child_index, console::TABLE_ALIGN_MIDDLE },
					{ "" }
					});
				tb.Print();
			}

			//single iteration.
			void MctsIteration(Node* root_node, Allocator* allocator)
			{
				timer::TimePoint tp;
				size_t iteration_time = _setting.max_iteration_per_thread;
				for (size_t i = 0; i < iteration_time; i++)
				{
					//stop search if timout.
					if (timeout(tp, _setting.timeout))
						return; 

					//excute garbage collection if need.
					//TODO

					//run out of memory, stop search.
					if (allocator->is_full())
						return;

					//excute next.
					root_node->Selection(*allocator, _func_package, _setting);
				}
			}

			//excute mcts iterations.
			virtual void ExcuteIteration(Node& root_node, stl::LinearAllocator<Allocator, _is_debug>& allocators) = 0;

			//excute iteration function.
			Action ExcuteMCTS(const State& root_state)
			{
				//thread num must can not be 0.
				if (_setting.max_thread == 0)
				{
					_setting.max_thread = 1;
					if (log_enabled())
						logger() << ">> [WARNING]: max_thread can not be 0, which had been changed to 1." << std::endl;
				}
					

				timer::TimePoint tp_start;
				Node root_node(root_state, nullptr, _func_package, _setting);
				stl::LinearAllocator<Allocator, _is_debug> allocators(_setting.max_thread);

				//outputt log if enabled.
				if (log_enabled())
				{
					logger() << "[ "<< name() <<" ]" << std::endl;
					_setting.PrintInfo();
					logger() << std::endl << ">> Executing......" << std::endl;
				}
				
				//return action if there is only one action in root node.
				if (root_node.action_num() == 1)
				{
					if (log_enabled())
					{
						logger() << ">> Only one action is available. action = " << _log_controller.action_to_str_func()(root_node.action(0)) << std::endl;
					}
					return root_node.action(0);
				}

				//excute iterations.
				ExcuteIteration(root_node, allocators);

				//select best action.
				GADT_CHECK_WARNING(is_debug(), root_node.fir_child_node() == nullptr, "empty child node under root node.");
				GADT_CHECK_WARNING(is_debug(), root_node.action_num() == 0, "no existing available action in root node.");
				size_t best_child_index = _func_package.RootSelection(root_node);

				//output Json if enabled.
				if (json_output_enabled())
				{
					JsonConvert(&root_node, &_log_controller.visual_tree(), _log_controller.state_to_str_func());
					_log_controller.OutputJson();
				}

				//print log if enabled
				if (log_enabled())
				{
					PrintResult(root_node, best_child_index, tp_start);
				}
				
				return root_node.action(best_child_index);
			}

		public:

			//create monte carlo tree search with necessary functions. 
			MctsBase(
				typename FuncPackage::UpdateStateFunc		_UpdateState,
				typename FuncPackage::MakeActionFunc		_MakeAction,
				typename FuncPackage::DetermineWinnerFunc	_DetermineWinner,
				typename FuncPackage::StateToResultFunc		_StateToResult,
				typename FuncPackage::AllowUpdateValueFunc	_AllowUpdateValue
			):
				GameAlgorithmBase<State, Action, Result, _is_debug>(""),
				_func_package( _UpdateState, _MakeAction, _DetermineWinner, _StateToResult, _AllowUpdateValue),
				_setting()
			{
			}


			//create monte carlo tree search with function package. 
			MctsBase(FuncPackage function_package, MctsSetting setting) :
				GameAlgorithmBase<State, Action, Result, _is_debug>(""),
				_func_package(function_package),
				_setting(setting)
			{
			}

			inline void Set(MctsSetting setting)
			{
				_setting = setting;
			}

			//run mcts.
			virtual Action Run(const State& root_state) = 0;

			//run mcts with custom setting.
			virtual Action Run(const State& root_state, MctsSetting setting) = 0;
		};

		/*
		* class MonteCarloTreeSearch is a basic template of MCTS, which do not use multi-thread.
		*
		* [State] is the game-state class, which is defined by the user.
		* [Action] is the game-action class, which is defined by the user.
		* [Result] is the game-result class, which stand for a terminal state of the game.
		* [_is_debug] decides whether debug info would be ignored or not. which may cause slight degradation in performance if it is enabled.
		*/
		template<typename State, typename Action, typename Result = AgentIndex, bool _is_debug = false>
		class MonteCarloTreeSearch final : MctsBase<State,Action,Result, _is_debug>
		{
		public:

			using GameAlgorithmBase<State, Action, Result, _is_debug>::name;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::set_name;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::InitLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::EnableLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::DisableLog;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::EnableJsonOutput;
			using GameAlgorithmBase<State, Action, Result, _is_debug>::DisableJsonOutput;
			using MctsBase<State, Action, Result, _is_debug>::_func_package;
			using MctsBase<State, Action, Result, _is_debug>::_setting;
			using MctsBase<State, Action, Result, _is_debug>::Set;
			using MctsBase<State, Action, Result, _is_debug>::MctsIteration;
			using MctsBase<State, Action, Result, _is_debug>::ExcuteIteration;
			using MctsBase<State, Action, Result, _is_debug>::ExcuteMCTS;

		private:

			using Node = MctsNode<State, Action, Result, _is_debug>;				//searcg node.	
			using Allocator = typename Node::Allocator;								//allocator of nodes
			using FuncPackage = MctsFuncPackage<State, Action, Result, _is_debug>;	//function package.

		private:

			//excute mcts iterations.
			void ExcuteIteration(Node& root_node, stl::LinearAllocator<Allocator, _is_debug>& allocators) override
			{
				//thread content
				std::vector<std::thread> threads;

				//run iterations.
				Allocator* allocator = allocators.construct(_setting.max_node_per_thread);
				MctsIteration(&root_node, allocator);

				//delete the original visit of root node.
				root_node.set_visit_count(root_node.visit_count() - 1);
			}

		public:

			//create monte carlo tree search with necessary functions. 
			MonteCarloTreeSearch(
				typename FuncPackage::UpdateStateFunc		_UpdateState,
				typename FuncPackage::MakeActionFunc		_MakeAction,
				typename FuncPackage::DetermineWinnerFunc	_DetermineWinner,
				typename FuncPackage::StateToResultFunc		_StateToResult,
				typename FuncPackage::AllowUpdateValueFunc	_AllowUpdateValue
			) :
				MctsBase<State, Action, Result, _is_debug>(_UpdateState, _MakeAction, _DetermineWinner, _StateToResult, _AllowUpdateValue)
			{
				set_name("Monte Carlo Tree Search");
			}

			//create monte carlo tree search with function package. 
			MonteCarloTreeSearch(FuncPackage function_package, MctsSetting setting = MctsSetting()) :
				MctsBase<State, Action, Result, _is_debug>(function_package, setting)
			{
				set_name("Monte Carlo Tree Search");
			}

			//run mcts.
			Action Run(const State& root_state) override
			{
				_setting.max_thread = 1;
				return ExcuteMCTS(root_state);
			}

			//run mcts with custom setting.
			Action Run(const State& root_state, MctsSetting setting) override
			{
				_setting.max_thread = 1;
				return ExcuteMCTS(root_state);
			}
		};
	}
}