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

#pragma once

namespace gadt
{
	namespace minimax
	{
		//allow check warning if it is true.
		constexpr const bool g_ENABLE_MINIMAX_WARNING = true;

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

			//default setting constructor.
			MinimaxSetting() :
				timeout(30),
				max_depth(10),
				ab_prune_enabled(false)
			{
			}

			//custom setting constructor.
			MinimaxSetting(double _timeout, size_t _max_depth, bool _ab_prune_enabled) :
				timeout(_timeout),
				max_depth(_max_depth),
				ab_prune_enabled(_ab_prune_enabled)
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
				const AgentIndex			no_winner_index;	//the index of no winner
				const GetNewStateFunc		GetNewState;		//get a new state from previous state and action.
				const MakeActionFunc		MakeAction;			//the function which create action set by the state.
				const DetemineWinnerFunc	DetemineWinner;		//return no_winner_index if a state is not terminal state.
				const EvalForParentFunc		EvalForParent;		//get the eval for parent node.

			public:
				FuncPackage(
					AgentIndex				_no_winner_index,
					GetNewStateFunc			_GetNewState,
					MakeActionFunc			_MakeAction,
					DetemineWinnerFunc		_DetemineWinner,
					EvalForParentFunc		_EvalForParent
				) :
					no_winner_index(_no_winner_index),
					GetNewState(_GetNewState),
					MakeAction(_MakeAction),
					DetemineWinner(_DetemineWinner),
					EvalForParent(_EvalForParent)
				{
				}
			};

		private:
			const State			_state;		//game state
			const size_t		_depth;		//depth of the node
			ActionSet			_actions;	//action set
			AgentIndex			_winner;	//winner of the node.
			const FuncPackage& _params;	//parameters package

		private:
			inline void NodeInit()
			{
				_winner = _params.DetemineWinner(_state);
				_params.MakeAction(_state, _actions);
			}

		public:

			//constructor for root node
			MinimaxNode(const State& state, const FuncPackage& params):
				_state(state),
				_depth(0),
				_actions(),
				_winner(),
				_params(params)
			{
				NodeInit(params);
			}

			//constructor for child node.
			MinimaxNode(const State& parent_state, const Action& taken_action, size_t depth, const FuncPackage& params):
				_state(params.GetNewState(parent_state,taken_action)),
				_depth(depth),
				_actions(),
				_winner(),
				_params(params)
			{
				NodeInit(params);
			}

			//return true if the node is terminal.
			inline bool is_terminal_state() const
			{
				return _winner != _params.no_winner_index;
			}

			//get value of this node for parent node.
			EvalValue GetHighestValueForParent() const
			{
				if (_depth == 0 || is_terminal_state())
				{
					return _params.EvalForParent(_state, _winner);
				}
				GADT_CHECK_WARNING(g_ENABLE_MINIMAX_WARNING, _actions.size() == 0, "MM101: empty action set");
				Node first_child(_state, _actions[0], _depth - 1, _params);
				EvalValue best_value = first_child.GetValue();
				for (size_t i = 1; i < _actions.size(); i++)
				{
					Node child(_state, _actions[i], _depth - 1, _params);
					EvalValue child_value = child.GetValueForParent();
					if (child_value >= best_value)
					{
						best_value = child_value;
					}
				}
				return best_value;
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
			using VisualNode	= visual_tree::VisualNode;
			
		private:
			
			FuncPackage		_func_package;
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

		public:
			//constructor func.
			MinimaxSearch(FuncPackage params):
				_params(params),
				_visual_tree(),
				_log_controller()
			{
			}

			//TODO
			//excute nega minimax search
			Action DoNegamax(const State& state, MinimaxSetting setting = MinimaxSetting())
			{
				_setting = setting;

			}

			//TODO
			//excute alpha-beta search.
			Action DoAlphaBeta(const State& state)
			{
				return Action();
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
