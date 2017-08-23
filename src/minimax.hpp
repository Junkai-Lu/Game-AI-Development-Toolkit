/*
* General Minimax algorithm lib.
*
* this lib is available for perfect information sequential games.
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
#include "gadtstl.hpp"

#pragma once

namespace gadt
{
	namespace minimax
	{
		//allow check warning if it is true.
		constexpr const bool g_MINIMAX_ENABLE_WARNING = true;

		//AgentIndex is the index of each player, default is int8_t. 0 is the default no-winner index.
		using AgentIndex = int8_t;
		using EvalValue = double;

		/*
		* MinimaxSetting is the setting of MCTS.
		*
		* MinimaxSetting() would use default setting.
		* MinimaxSetting(params) would generate custom setting.
		*/
		struct MinimaxSetting
		{
			double timeout;
			size_t max_depth;
			bool ab_prune_enabled;
			AgentIndex no_winner_index;	//the index of no winner
			EvalValue original_eval; //original evale value.

			//default setting constructor.
			MinimaxSetting() :
				timeout(30),
				max_depth(10),
				ab_prune_enabled(false),
				no_winner_index(0)
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
				timeout(_timeout),
				max_depth(_max_depth),
				ab_prune_enabled(_ab_prune_enabled),
				no_winner_index(_no_winner_index)
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
			using pointer = MinimaxNode<State, Action, _is_debug>*;
			using reference = MinimaxNode<State, Action, _is_debug>&;
			using Node = MinimaxNode<State, Action, _is_debug>;
			using ActionSet = std::vector<Action>;

			struct FuncPackage
			{
			public:
				using GetNewStateFunc = std::function<State(const State&, const Action&)>;
				using MakeActionFunc = std::function<void(const State&, ActionSet&)>;
				using DetemineWinnerFunc = std::function<AgentIndex(const State&)>;
				using EvalForParentFunc = std::function<EvalValue(const State&, const AgentIndex)>;

			public:
				//necessary functions.
				const GetNewStateFunc		GetNewState;		//get a new state from previous state and action.
				const MakeActionFunc		MakeAction;			//the function which create action set by the state.
				const DetemineWinnerFunc	DetemineWinner;		//return no_winner_index if a state is not terminal state.
				const EvalForParentFunc		EvalForParent;		//get the eval for parent node.

			public:
				FuncPackage(
					GetNewStateFunc			_GetNewState,
					MakeActionFunc			_MakeAction,
					DetemineWinnerFunc		_DetemineWinner,
					EvalForParentFunc		_EvalForParent
				) :
					GetNewState(_GetNewState),
					MakeAction(_MakeAction),
					DetemineWinner(_DetemineWinner),
					EvalForParent(_EvalForParent)
				{
				}
			};

		private:
			const State			  _state;	//game state
			const size_t		  _depth;	//depth of the node
			ActionSet			  _actions;	//action set
			AgentIndex			  _winner;	//winner of the node.
			const FuncPackage&	  _func;    //function package
			const MinimaxSetting& _setting; //setting package.

		private:

			//
			inline void NodeInit()
			{
				_winner = _func.DetemineWinner(_state);
				_func.MakeAction(_state, _actions);
			}

		public:

			//constructor for root node
			MinimaxNode(const State& state, const FuncPackage& func, const MinimaxSetting& setting):
				_state(state),
				_depth(setting.max_depth),
				_actions(),
				_winner(),
				_func(func),
				_setting(setting)
			{
				NodeInit();
			}

			//constructor for child node.
			MinimaxNode(const State& parent_state, const Action& taken_action, size_t depth, const FuncPackage& func, const MinimaxSetting& setting):
				_state(func.GetNewState(parent_state,taken_action)),
				_depth(depth),
				_actions(),
				_winner(),
				_func(func),
				_setting(setting)
			{
				NodeInit();
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
			inline const ActionSet& action_set() const
			{
				return _actions;
			}

			//get winner of state.
			inline AgentIndex winner() const
			{
				return _winner;
			}

			//return true if the node is terminal.
			inline bool is_terminal_state() const
			{
				return _winner != _setting.no_winner_index;
			}
		};

		/*
		* MinimaxSearch is a template of Minimax search.
		*
		* [State] is the game-state class, which is defined by the user.
		* [Action] is the game-action class, which is defined by the user.
		* [_is_debug] means some debug info would not be ignored if it is true. this may result in a little degradation of performance.
		*/
		template<typename State, typename Action, bool _is_debug = false>
		class MinimaxSearch
		{
		public:
			using Node			= MinimaxNode<State, Action, _is_debug>;
			using ActionSet		= typename Node::ActionSet;
			using FuncPackage	= typename Node::FuncPackage;
			using LogController	= log::SearchLogger<State, Action>;
			using VisualTree	= visual_tree::VisualTree;
			using VisualNodePtr	= visual_tree::VisualNode*;
			
		private:
			
			FuncPackage		_func;
			MinimaxSetting	_setting;
			LogController	_log_controller;

		private:

			//return true if is debug.
			constexpr bool is_debug() const
			{
				return _is_debug;
			}

			//get info of this search.
			std::string info() const
			{
				std::stringstream ss;
				ss << std::boolalpha << "{" << std::endl
					<< "    timeout: " << _setting.timeout << std::endl
					<< "    max_depth: " << _setting.max_depth << std::endl
					<< "    ab_prune_enabled: " << _setting.ab_prune_enabled << std::endl
					<< "}" << std::endl;
				return ss.str();
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

			//convert minimax node to visual node.
			void MinimaxNodeToVisualNode(const Node& node, VisualNodePtr visual_node)
			{
				static constexpr const char* STATE_NAME = "state";
				static constexpr const char* DEPTH_NAME = "depth";
				static constexpr const char* WINNER_NAME = "winner";
				static constexpr const char* IS_TERMINAL_STATE_NAME = "is_terminal_state";

				visual_node->add_value(STATE_NAME, _log_controller.state_to_str_func()(node.state()));
				visual_node->add_value(DEPTH_NAME, node.depth());
				visual_node->add_value(WINNER_NAME, node.winner());
				visual_node->add_value(IS_TERMINAL_STATE_NAME, node.is_terminal_state());
			}

			//get the highest eval for parent of node.
			EvalValue NegamaxEvalForParents(const Node& node, VisualNodePtr parent_visual_node, size_t& leaf_node_count)
			{
				
				VisualNodePtr visual_node = nullptr;
				const size_t original_leaf_node_count = leaf_node_count;

				if (json_output_enabled())
				{
					visual_node = parent_visual_node->create_child();
					MinimaxNodeToVisualNode(node, visual_node);
				}

				if (node.depth() == 0 || node.is_terminal_state())
				{
					//get the eavl of the parent node.
					leaf_node_count++;
					EvalValue eval = _func.EvalForParent(node.state(), node.winner());

					if (json_output_enabled())
					{
						visual_node->add_value("eval", eval);
					}

					return eval;
				}

				if (is_debug())
				{
					GADT_CHECK_WARNING(g_MINIMAX_ENABLE_WARNING, node.action_set().size() == 0, "MM101: empty action set");
				}
				
				//pick up best value in child nodes.
				Node first_child(node.state(), node.action_set()[0], node.depth() - 1, _func,_setting);
				EvalValue best_value = NegamaxEvalForParents(first_child, visual_node, leaf_node_count);
				for (size_t i = 1; i < node.action_set().size(); i++)
				{
					Node child(node.state(), node.action_set()[i], node.depth() - 1, _func, _setting);
					EvalValue child_value = NegamaxEvalForParents(child, visual_node, leaf_node_count);
					if (child_value >= best_value) { best_value = child_value; }
				}

				if (json_output_enabled())
				{
					visual_node->add_value("leaf node count", leaf_node_count - original_leaf_node_count);
					visual_node->add_value("value for parent", -best_value);
				}
				return -best_value;
			}

		public:
			//constructor func.
			MinimaxSearch(
				typename FuncPackage::GetNewStateFunc		GetNewState,
				typename FuncPackage::MakeActionFunc		MakeAction,
				typename FuncPackage::DetemineWinnerFunc	DetemineWinner,
				typename FuncPackage::EvalForParentFunc		EvalForParent
			):
				_func(GetNewState,MakeAction,DetemineWinner,EvalForParent),
				_setting(),
				_log_controller()
			{
			}

			//excute nega minimax search
			Action DoNegamax(const State& state, MinimaxSetting setting = MinimaxSetting())
			{
				_setting = setting;
				Node root(state, _func, _setting);
				VisualNodePtr root_visual_node = nullptr;

				if (is_debug())
				{
					GADT_CHECK_WARNING(g_MINIMAX_ENABLE_WARNING, root.is_terminal_state(), "MM102: execute search for terminal state.");
				}

				if (log_enabled())
				{
					logger() << ">> Minimax Search start" << std::endl;
				}

				if (json_output_enabled())
				{
					root_visual_node = _log_controller.visual_tree().root_node();
				}

				std::vector<EvalValue> eval_set(root.action_set().size());
				size_t leaf_node_count = 0;

				//pick up best action.
				Node first_child(state, root.action_set()[0], root.depth() - 1, _func, _setting);
				EvalValue best_value = NegamaxEvalForParents(first_child, root_visual_node, leaf_node_count);
				eval_set[0] = best_value;
				size_t best_action_index = 0;
				for (size_t i = 1; i < root.action_set().size(); i++)
				{
					Node child(state, root.action_set()[i], root.depth() - 1, _func, _setting);
					eval_set[i] = NegamaxEvalForParents(child, root_visual_node, leaf_node_count);
					
					if (eval_set[i] > best_value)
					{
						best_action_index = i;
						best_value = eval_set[i];
					}
				}
				

				if (log_enabled())
				{
					table::ConsoleTable tb(4, root.action_set().size() + 1);
					tb.set_cell_in_row(0, { {"Index" }, {"Action"},{"Eval"},{"Is Best"} });
					tb.set_width({ 3,10,4,4 });
					for (size_t i = 0; i < root.action_set().size(); i++)
					{
						tb.set_cell_in_row(i + 1, {
							{ console::IntergerToString(i) },
							{ _log_controller.action_to_str_func()(root.action_set()[i])},
							{ console::DoubleToString(eval_set[i])},
							{ i == best_action_index ? "Y ":"  "}
						});
					}
					logger() << tb.output_string(true, false) << std::endl;
				}

				if (json_output_enabled())
				{
					root_visual_node->add_value("leaf node count", leaf_node_count);
					_log_controller.OutputJson();
				}

				//return best action.
				return root.action_set()[best_action_index];
			}

			//TODO
			//excute alpha-beta search.
			Action DoAlphaBeta(const State& state, MinimaxSetting setting = MinimaxSetting())
			{
				_setting = setting;
			}

			//enable log
			void InitLog(typename LogController::StateToStrFunc StateToStr, typename LogController::ActionToStrFunc ActionToStr)
			{
				_log_controller.Init(StateToStr, ActionToStr);
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
			inline void EnableJsonOutput(std::string output_path = "./MinimaxOutput.json")
			{
				_log_controller.EnableJsonOutput(output_path);
			}

			//disable json output.
			inline void DisableJsonOutput()
			{
				_log_controller.DisableJsonOutput();
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
