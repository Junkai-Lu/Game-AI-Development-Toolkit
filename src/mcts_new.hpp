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
		//MctsResult is the simulation result in the mcts. it include the index of winner and the final state of the game;
		template<typename state_type, bool enable_debug_model = false>
		class MctsResult
		{
		public:
			using AgentIndex = int8_t;
			using State = state_type;

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


		template<typename state_type, typename action_type, bool enable_debug_model = false>
		class MctsNode
		{
		public:
			using AgentIndex = int8_t;
			using State = state_type;
			using Action = action_type;
			using ActionSet = std::vector<Action>;
			using MakeActionFunc = std::function<void(const State&, ActionSet&)>;
			using CheckResultFunc = std::function<int(const State&)>;
			

		private:
			State		_state;				//state of this node.
			ActionSet	_action_set;		//action set of this node.
			uint8_t		_next_action_index;	//the index of next action.
			AgentIndex	_winner;			//the winner of the state.

		public:

		};



	}
}