/* Copyright (c) 2018 Junkai Lu <junkai-lu@outlook.com>.
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

		struct BtAction
		{
			UPoint source;
			UPoint dest;

			std::string to_string() const
			{
				std::stringstream ss;
				ss << "from " << source.to_string() << " to " << dest.to_string();
				return ss.str();
			}
		};

		using BtActionList = std::vector<BtAction>;

		class BtState
		{
		private:
			using Board = stl::StaticMatrix<BtPlayer, g_BT_WIDTH, g_BT_HEIGHT>;

		private:
			Board _board;
			BtPlayer _next_player;
			size_t _white_piece;
			size_t _black_piece;

		private:

			inline void init_piece()
			{
				_board.set_row(0, WHITE);
				_board.set_row(1, WHITE);
				_board.set_row(g_BT_HEIGHT - 1, BLACK);
				_board.set_row(g_BT_HEIGHT - 2, BLACK);
			}

			inline void reduce_piece(BtPlayer piece)
			{
				if (piece == BLACK)
					_black_piece--;
				if (piece == WHITE)
					_white_piece--;
			}

		public:

			const Board& board() const { return _board; }
			BtPlayer next_player() const { return _next_player; }
			size_t remain_piece(BtPlayer player) const { return player == BLACK ? _black_piece : _white_piece; }
			BtPlayer piece(UPoint point) const { return _board.element(point); }

		public:

			BtState():
				_board(NO_PLAYER),
				_black_piece(2 * g_BT_WIDTH),
				_white_piece(2 * g_BT_WIDTH),
				_next_player(BLACK)
			{
				init_piece();
			}

			BtState(const BtState& state, const BtAction& action)
			{
				*this = state;
				TakeAction(action);
			}

			void TakeAction(const BtAction& action)
			{
				if (piece(action.dest) != NO_PLAYER)
					reduce_piece(piece(action.dest));
				_board[action.dest] = _board[action.source];
				_board[action.source] = NO_PLAYER;
				_next_player = (_next_player == BLACK) ? WHITE : BLACK;
			}

			void Print() const
			{
				console::Table table(g_BT_WIDTH, g_BT_HEIGHT);
				for (auto point : _board)
				{
					switch (_board.element(point))
					{
					case WHITE:
						table.set_cell({ "W " , console::ConsoleColor::Red, console::TableAlign::Middle}, point);
						break;
					case BLACK:
						table.set_cell({ "B " , console::ConsoleColor::Blue, console::TableAlign::Middle}, point);
						break;
					default:
						break;
					}
				}
				table.Print(console::TableFrame::Enable, console::TableIndex::BeginFromZero);
			}

			BtPlayer Winner() const
			{
				for (size_t i = 0; i < g_BT_WIDTH; i++)
				{
					if (_board.element(i, 0) == BLACK)
						return BLACK;
					if (_board.element(i, g_BT_HEIGHT - 1) == WHITE)
						return WHITE;
				}
				if (_black_piece == 0)
					return WHITE;
				if (_white_piece == 0)
					return BLACK;
				return NO_PLAYER;
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
				BtActionList actions;
				BtPlayer player = _state.next_player();
				int dir = _state.next_player() == BLACK ? -1 : 1;
				Point right_move = { 1,dir };
				Point left_move = { -1, dir };
				Point center_move = { 0,dir };

				for (auto point : _state.board())
				{
					const auto signed_point = point.to_signed();
					if (_state.piece(point) == player)
					{
						UPoint right_point = (signed_point + right_move).to_unsigned();
						if (_state.board().is_legal_point(right_point) && _state.piece(right_point) != player)
							actions.push_back({ point, right_point });
						UPoint left_point = (signed_point + left_move).to_unsigned();
						if (_state.board().is_legal_point(left_point) && _state.piece(left_point) != player)
							actions.push_back({ point, left_point });
						UPoint center_point = (signed_point + center_move).to_unsigned();
						if (_state.board().is_legal_point(center_point) && _state.piece(center_point) == NO_PLAYER)
							actions.push_back({ point, center_point });
					}
				}
				return actions;
			}

			void Print() const
			{
				size_t index = 0;
				for (auto act : GetAllAction())
				{
					std::cout << index++ << " : " << act.to_string() << std::endl;
				}
			}
		};

		void UpdateState(BtState& state, const BtAction& action);
		void MakeAction(const BtState& state, BtActionList& list);
		BtPlayer DetemineWinner(const BtState& state);
		BtPlayer StateToResult(const BtState& state, AgentIndex winner);
		bool AllowUpdateValue(const BtState& state, BtPlayer winner);
		std::string StateToString(const BtState& state);
		std::string ActionToString(const BtAction& action);
		std::string ResultToString(const BtPlayer& result);

		void DefineBreakthroughShell(shell::page::PagePtr<int> page_ptr);
	}
}