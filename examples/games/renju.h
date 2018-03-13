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

#include "../../src/gadtlib.h"
#include "../../src/monte_carlo.hpp"
#include "../../src/gadt_game.hpp"
#include "../../src/game_shell.h"
#include "../../src/mcts.hpp"

#pragma once

namespace gadt
{
	namespace renju
	{
		constexpr const size_t g_WIDTH = 15;
		constexpr const size_t g_HEIGHT = 15;
		constexpr const size_t g_LENGTH = 5;

		enum RenjuPlayer: AgentIndex
		{
			WHITE = -1,
			EMPTY = 0,
			BLACK = 1,
			DRAW = 10,
			FLAG = 20

		};

		using RenjuState = game::MnkGameState<g_WIDTH, g_HEIGHT, g_LENGTH>;
		using RenjuAction = game::MnkGameAction;
		using RenjuActionList = std::vector<RenjuAction>;
		using RenjuResult = RenjuPlayer;
		using RenjuActionGenerator = game::MnkGameActionGenerator<g_WIDTH, g_HEIGHT, g_LENGTH>;

		void UpdateState(RenjuState& state, const RenjuAction& action);

		void MakeAction(const RenjuState& state, RenjuActionList& list);

		RenjuPlayer DetemineWinner(const RenjuState& state);

		RenjuResult StateToResult(const RenjuState& state, AgentIndex winner);

		bool AllowUpdateValue(const RenjuState& state, RenjuResult winner);

		std::string StateToString(const RenjuState& state);

		std::string ActionToString(const RenjuAction& action);

		std::string ResultToString(const RenjuResult& result);

		void PrintRenjuState(const RenjuState& state);

		void PrintRenjuActions(const RenjuState& state, RenjuActionList action_list);

		using RenjuMCTS = mcts::MonteCarloTreeSearch<RenjuState, RenjuAction, RenjuResult, true>;

		void DefineRenjuShell(shell::GameShell& shell);
	}
}