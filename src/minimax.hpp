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
	namespace minimax
	{
		/*
		* MinimaxSetting is the setting of MCTS.
		*
		* MinimaxSetting() would use default setting.
		* MinimaxSetting(params) would generate custom setting.
		*/
		struct MinimaxSetting final : GameAlgorithmSettingBase
		{
			size_t max_depth;
			bool ab_prune_enabled;
			EvalValue original_eval;		//original eval value.

			//default setting constructor.
			MinimaxSetting() :
				GameAlgorithmSettingBase(),
				max_depth(10),
				ab_prune_enabled(false),
				original_eval(0)
			{
			}

			//custom setting constructor.
			MinimaxSetting(
				double _timeout, 
				size_t _max_depth, 
				bool _ab_prune_enabled, 
				AgentIndex _no_winner_index = 0, 
				EvalValue _original_eval = 0
			) :
				GameAlgorithmSettingBase(_timeout, _no_winner_index),
				max_depth(_max_depth),
				ab_prune_enabled(_ab_prune_enabled),
				original_eval(_original_eval)
			{
			}

			void PrintInfo() const override
			{
				console::Table tb(2, 5);
				tb.set_width({ 12,6 });
				tb.enable_title({ "MINIMAX SETTING" });
				tb.set_cell_in_row(0, { { "timeout" },			{ ToString(timeout) } });
				tb.set_cell_in_row(1, { { "max_depth" },		{ ToString(max_depth) } });
				tb.set_cell_in_row(2, { { "ab_prune_enabled" },	{ ToString(ab_prune_enabled) } });
				tb.set_cell_in_row(3, { { "no_winner_index" },	{ ToString(no_winner_index) } });
				tb.set_cell_in_row(4, { { "original_eval" },	{ ToString(original_eval) } });
				tb.Print();
			}
		};

		template<typename State, typename Action, bool _is_debug>
		struct MinimaxFuncPackage final: public GameAlgorithmFuncPackageBase<State, Action, _is_debug>
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
			using EvalForParentFunc = std::function<EvalValue(const State&, const AgentIndex)>;

		public:
			//necessary functions.
			const EvalForParentFunc		EvalForParent;		//get the eval for parent node.

		public:
			MinimaxFuncPackage(
				UpdateStateFunc			_UpdateState,
				MakeActionFunc			_MakeAction,
				DetermineWinnerFunc		_DetermineWinner,
				EvalForParentFunc		_EvalForParent
			) :
				GameAlgorithmFuncPackageBase<State, Action, _is_debug>(_UpdateState, _MakeAction, _DetermineWinner),
				EvalForParent(_EvalForParent)
			{
			}
		};

		/*
		* MinimaxNode is the node class in the minimax search.
		*
		* [State] is the game-state class, which is defined by the user.
		* [Action] is the game-action class, which is defined by the user.
		* [is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename State, typename Action, bool _is_debug>
		class MinimaxNode
		{
		public:
			using pointer     = MinimaxNode<State, Action, _is_debug>*;
			using reference   = MinimaxNode<State, Action, _is_debug>&;
			using Node        = MinimaxNode<State, Action, _is_debug>;
			using FuncPackage = MinimaxFuncPackage<State, Action, _is_debug>;
			using ActionList  = typename FuncPackage::ActionList;

		private:
			const State			  _state;		//game state
			const size_t		  _depth;		//depth of the node
			ActionList			  _action_list;	//action set
			AgentIndex			  _winner;		//winner of the node.

		private:

			//node initialize
			inline void NodeInit(const FuncPackage& func_package)
			{
				_winner = func_package.DetermineWinner(_state);
				func_package.MakeAction(_state, _action_list);
			}

		public:
			//constructor
			MinimaxNode(const State& new_state, size_t depth, const FuncPackage& func_package):
				_state(new_state),
				_depth(depth),
				_action_list(),
				_winner()
			{
				NodeInit(func_package);
			}

			//get state of the node.
			inline const State& state() const
			{
				return _state;
			}

			//get depth of current node.
			inline size_t depth() const
			{
				return _depth;
			}

			//get the action set of the node.
			inline const ActionList& action_list() const
			{
				return _action_list;
			}

			//get winner of state.
			inline AgentIndex winner() const
			{
				return _winner;
			}

			//return true if the node is terminal.
			inline bool is_terminal_state(const MinimaxSetting& setting) const
			{
				return _winner != setting.no_winner_index;
			}
		};

		/*
		* MinimaxSearch is a template of Minimax search.
		*
		* [State] is the game-state class, which is defined by the user.
		* [Action] is the game-action class, which is defined by the user.
		* [_is_debug] decides whether debug info would be ignored or not. which may cause slight degradation in performance if it is enabled.
		*/
		template<typename State, typename Action, bool _is_debug = false>
		class MinimaxSearch final : public GameAlgorithmBase<State, Action, AgentIndex, _is_debug>
		{

		private:
			using GameAlgorithmBase<State, Action, AgentIndex, _is_debug>::_log_controller;
			using GameAlgorithmBase<State, Action, AgentIndex, _is_debug>::logger;
			using GameAlgorithmBase<State, Action, AgentIndex, _is_debug>::log_enabled;
			using GameAlgorithmBase<State, Action, AgentIndex, _is_debug>::json_output_enabled;
			using GameAlgorithmBase<State, Action, AgentIndex, _is_debug>::timeout;
			using GameAlgorithmBase<State, Action, AgentIndex, _is_debug>::is_debug;

		public:
			using GameAlgorithmBase<State, Action, AgentIndex, _is_debug>::name;
			using GameAlgorithmBase<State, Action, AgentIndex, _is_debug>::set_name;
			using GameAlgorithmBase<State, Action, AgentIndex, _is_debug>::InitLog;
			using GameAlgorithmBase<State, Action, AgentIndex, _is_debug>::EnableLog;
			using GameAlgorithmBase<State, Action, AgentIndex, _is_debug>::DisableLog;
			using GameAlgorithmBase<State, Action, AgentIndex, _is_debug>::EnableJsonOutput;
			using GameAlgorithmBase<State, Action, AgentIndex, _is_debug>::DisableJsonOutput;

		public:
			using Node			= MinimaxNode<State, Action, _is_debug>;
			using FuncPackage	= typename Node::FuncPackage;
			using ActionList	= typename FuncPackage::ActionList;
			using VisualTree	= visual_tree::VisualTree;
			using VisualNodePtr	= visual_tree::VisualNode*;
			
		private:
			
			FuncPackage		_func_package;
			MinimaxSetting	_setting;

		private:
			//convert minimax node to visual node.
			void MinimaxNodeToVisualNode(const Node& node, VisualNodePtr visual_node, const MinimaxSetting& setting)
			{
				static constexpr const char* STATE_NAME = "state";
				static constexpr const char* DEPTH_NAME = "depth";
				static constexpr const char* WINNER_NAME = "winner";
				static constexpr const char* IS_TERMINAL_STATE_NAME = "is_terminal_state";

				visual_node->add_value(STATE_NAME, _log_controller.state_to_str_func()(node.state()));
				visual_node->add_value(DEPTH_NAME, node.depth());
				visual_node->add_value(WINNER_NAME, node.winner());
				visual_node->add_value(IS_TERMINAL_STATE_NAME, node.is_terminal_state(setting));
			}

			//get the highest eval for parent of node.
			EvalValue NegamaxEvalForParents(const Node& node, VisualNodePtr parent_visual_node, size_t& leaf_node_count)
			{
				VisualNodePtr visual_node = nullptr;
				const size_t original_leaf_node_count = leaf_node_count;

				if (json_output_enabled())
				{
					visual_node = parent_visual_node->create_child();
					MinimaxNodeToVisualNode(node, visual_node, _setting);
				}

				if (node.depth() == 0 || node.is_terminal_state(_setting))
				{
					//get the eavl of the parent node.
					leaf_node_count++;
					EvalValue eval = _func_package.EvalForParent(node.state(), node.winner());

					if (json_output_enabled())
					{
						visual_node->add_value("eval for parent", eval);
					}
					return eval;
				}

				GADT_CHECK_WARNING(is_debug(), node.action_list().size() == 0, "MM101: empty action set");
				
				//pick up best value in child nodes.
				State first_child_state = node.state();
				_func_package.UpdateState(first_child_state, node.action_list()[0]);
				Node first_child(first_child_state, node.depth() - 1, _func_package);
				EvalValue best_value = NegamaxEvalForParents(first_child, visual_node, leaf_node_count);
				for (size_t i = 1; i < node.action_list().size(); i++)
				{
					State child_state = node.state();
					_func_package.UpdateState(child_state, node.action_list()[i]);
					Node child(child_state , node.depth() - 1, _func_package);
					EvalValue child_value = NegamaxEvalForParents(child, visual_node, leaf_node_count);
					if (child_value >= best_value) { best_value = child_value; }
				}

				EvalValue eval_for_parent = -best_value;//negamax!
				if (json_output_enabled())
				{
					visual_node->add_value("leaf node count", leaf_node_count - original_leaf_node_count);
					visual_node->add_value("eval for parent", eval_for_parent);
				}
				return eval_for_parent;
			}

		public:
			//constructor func.
			MinimaxSearch(
				typename FuncPackage::UpdateStateFunc		UpdateState,
				typename FuncPackage::MakeActionFunc		MakeAction,
				typename FuncPackage::DetermineWinnerFunc	DetermineWinner,
				typename FuncPackage::EvalForParentFunc		EvalForParent
			):
				GameAlgorithmBase<State, Action, AgentIndex, _is_debug>("Minimax"),
				_func_package(UpdateState,MakeAction,DetermineWinner,EvalForParent),
				_setting()
			{
			}

			//excute nega minimax search
			Action DoNegamax(const State& state, MinimaxSetting setting = MinimaxSetting())
			{
				_setting = setting;
				Node root(state, _setting.max_depth,_func_package);
				VisualNodePtr root_visual_node = nullptr;

				GADT_CHECK_WARNING(is_debug(), root.is_terminal_state(_setting), "MM102: execute search for terminal state.");

				if (log_enabled())
				{
					logger() << "[ Minimax Search ]" << std::endl;
					_setting.PrintInfo();
					logger() << std::endl << ">> Executing Minimax Search......" << std::endl;
				}

				if (json_output_enabled())
				{
					root_visual_node = _log_controller.visual_tree().root_node();
				}

				std::vector<EvalValue> eval_set(root.action_list().size());
				size_t leaf_node_count = 0;

				//pick up best action.
				
				//create first node.
				State first_child_state = state;
				_func_package.UpdateState(first_child_state, root.action_list()[0]);
				Node first_child(first_child_state, root.depth() - 1, _func_package);

				EvalValue best_value = NegamaxEvalForParents(first_child, root_visual_node, leaf_node_count);
				eval_set[0] = best_value;
				size_t best_action_index = 0;
				for (size_t i = 1; i < root.action_list().size(); i++)
				{
					//create new node.
					State child_state = state;
					_func_package.UpdateState(child_state, root.action_list()[i]);
					Node child(child_state, root.depth() - 1, _func_package);

					eval_set[i] = NegamaxEvalForParents(child, root_visual_node, leaf_node_count);
					
					if (eval_set[i] > best_value)
					{
						best_action_index = i;
						best_value = eval_set[i];
					}
				}

				if (log_enabled())
				{
					console::Table tb(4, root.action_list().size() + 1);
					tb.enable_title({ "MINIMAX RESULT" });
					tb.set_cell_in_row(0, { {"Index" }, {"Action"},{"Eval"},{"Is Best"} });
					tb.set_width({ 3,10,4,4 });
					for (size_t i = 0; i < root.action_list().size(); i++)
					{
						tb.set_cell_in_row(i + 1, {
							{ ToString(i) },
							{ _log_controller.action_to_str_func()(root.action_list()[i])},
							{ ToString(eval_set[i])},
							{ i == best_action_index ? "Yes ":"  "}
						});
					}
					tb.Print();
				}

				if (json_output_enabled())
				{
					root_visual_node->add_value("leaf node count", leaf_node_count);
					_log_controller.OutputJson();
				}

				//return best action.
				return root.action_list()[best_action_index];
			}

			//TODO
			//excute alpha-beta search.
			Action DoAlphaBeta(const State& state, MinimaxSetting setting = MinimaxSetting())
			{
				_setting = setting;
			}
		};

		/*
		* ExpectimaxSearch is a template of Expectimax search.
		*
		* [State] is the game-state class, which is defined by the user.
		* [Action] is the game-action class, which is defined by the user.
		* [_is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename State, typename Action, bool _is_debug = false>
		class ExpectimaxSearch
		{
			//TODO
		};
	}
}
