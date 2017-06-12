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

		/*
		* MctsNode is the node class in the monte carlo tree search.
		* [state_type] is the game-state class, which is defined by the user.
		* [action_type] is the game-action class, which is defined by the user.
		* [enable_debug_model] means enable the debug model, if it is enabled, some debug info would not be ignored. this may result in the degradation of performance.
		*/
		template<typename state_type, typename action_type, bool enable_debug_model = false>
		class MctsNode
		{
		public:											
			using State = state_type;												//State is the game-state class, which is defined by the user.
			using Action = action_type;												//Action is the game-action class which is defined by the user.
			using ActionSet = std::vector<Action>;									//ActionSet is the set of Action, each action would lead to a new state.
			using AgentIndex = int8_t;												//AgentIndex is the type index of each player, default is int8_t.
			using MakeActionFunc = std::function<void(const State&, ActionSet&)>;	//the function which create action set by the state.
			using CheckResultFunc = std::function<int(const State&)>;				//the function that check the simulation result in the backpropagation stage.

		private:
			State		_state;				//state of this node.
			ActionSet	_action_set;		//action set of this node.
			uint8_t		_next_action_index;	//the index of next action.
			AgentIndex	_winner_index;		//the winner index of the state.
			AgentIndex	_player_index;		//the belonging player index of this index.
			uint32_t	_visited_time;		//how many times that this node had been visited.
			uint32_t	_win_time;			//win time accmulated by the simulation.

		public:
			const State&		state()				const { return _state; }
			const ActionSet&	action_set()		const { return _action_set; }
			const uint8_t		next_action_index()	const { return _next_action_index; }
			const AgentIndex	winner_index()		const { return _winner_index; }
			const AgentIndex	player_index()		const { return _player_index; }
			const uint32_t		visited_time()		const { return _visited_time; }
			const uint32_t		win_time()			const { return _win_time; }

		private:
			//a value means no winner, which is differ from any other AgentIndex.
			static const AgentIndex _no_winner_index = 0;

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

			//return true if the state is the terminal-state of the game.
			inline bool is_end_state() const
			{
				return _winner_index != _no_winner_index;
			}

			

		};



	}
}