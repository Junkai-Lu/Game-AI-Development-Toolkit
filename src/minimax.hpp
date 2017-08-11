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

		//AgentIndex is the type index of each player, default is int8_t.
		using AgentIndex = int8_t;
		using EvalValue = double;

		

		template<typename State, typename Action, bool _is_debug>
		class MinimaxNode
		{
		public:
			using pointer = MinimaxNode<State, Action, _is_debug>*;
			using reference = MinimaxNode<State, Action, _is_debug>&;
			using Node = MinimaxNode<State, Action, _is_debug>;
			using ActionSet = std::vector<Action>;

			struct ParamPackage
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
				ParamPackage(
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
			const ParamPackage& _params;	//parameters package

		private:
			inline void NodeInit()
			{
				_winner = _params.DetemineWinner(_state);
				_params.MakeAction(_state, _actions);
			}

		public:

			//constructor for root node
			MinimaxNode(const State& state, const ParamPackage& params):
				_state(state),
				_depth(0),
				_actions(),
				_winner(),
				_params(params)
			{
				NodeInit(params);
			}

			//constructor for child node.
			MinimaxNode(const State& parent_state, const Action& taken_action, size_t depth, const ParamPackage& params):
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
				return _winnner != _params.no_winner_index;
			}

			//get value of this node for parent node.
			EvalValue GetValueForParent() const
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

		template<typename State, typename Action, bool _is_debug>
		class MinimaxSearch
		{
		public:
			using Node = MinimaxNode<State, Action, _is_debug>;
			using ActionSet = typename Node::ActionSet;
			using ParamPackage = typename Node::ParamPackage;

		private:
			
		public:
		};
	}
}
