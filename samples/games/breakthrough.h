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

#pragma once

#include "../../src/gadtlib.h"
#include "../../src/bitboard.hpp"
#include "../../src/monte_carlo.hpp"
#include "../../src/gadt_game.hpp"
#include "../../src/game_shell.h"
#include "../../src/mcts.hpp"


#pragma once

namespace gadt
{
	namespace breakthrough
	{
		constexpr const size_t g_BT_WIDTH = 8;
		constexpr const size_t g_BT_HEIGHT = 8;

		enum BtPlayer : AgentIndex
		{
			WHITE = -1,
			BLACK = 1,
			NO_PLAYER = 0
		};

		class BtAction
		{
			Coordinate source;
			Coordinate dest;
		};

		using BtActionList = std::vector<BtAction>;

		class BtState
		{
		private:
			using Board = stl::RectangularArray<BtPlayer, g_BT_WIDTH, g_BT_HEIGHT>;

		private:
			Board _board;

			inline void Init()
			{
				_board.set_row(0, WHITE);
				_board.set_row(1, WHITE);
				_board.set_row(g_BT_HEIGHT - 1, BLACK);
				_board.set_row(g_BT_HEIGHT - 2, BLACK);
			}

		public:

			BtState()
			{
				Init();
			}

			BtState(const BtState& state, const BtAction& action)
			{
				*this = state;
				TakeAction(action);
			}

			void TakeAction(const BtAction& action)
			{

			}

			void Print() const
			{
				log::ConsoleTable table(g_BT_WIDTH, g_BT_HEIGHT);
				for (auto coord : _board)
				{
					switch (_board.element(coord))
					{
					case WHITE:
						table.set_cell({ "¡ï" }, coord);
						break;
					case BLACK:
						table.set_cell({ "¡î" }, coord);
						break;
					default:
						break;
					}
				}
				table.print();
			}
		};

		class BtActionGenerator
		{
		private:
			const BtState& _state;

		public:
			BtActionGenerator(const BtState& state):
				_state(state)
			{
			}

			BtActionList GetAllAction() const
			{

			}
		};

		void DefineBreakthoughShell(shell::GameShell& shell);
	}
}