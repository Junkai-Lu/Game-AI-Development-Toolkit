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

#include "ewn.h"

namespace gadt
{
	namespace ewn
	{
		EwnState::EwnState():
			_board(0),
			_piece_flag(0),
			_next_player(RED),
			_roll_result(0)
		{
			Formation temp_red;
			Formation temp_blue;
			while (!(IsLegalFormation(temp_red) && IsLegalFormation(temp_blue)))
			{
				temp_red = InputFormation();
				temp_blue = InputFormation();
			}
			Init(temp_red, temp_blue);
		}

		EwnState::EwnState(Formation red, Formation blue) :
			_board(0),
			_piece_flag(0),
			_next_player(RED),
			_roll_result(0)
		{
			Formation temp_red = red;
			Formation temp_blue = blue;
			while (!(IsLegalFormation(temp_red) && IsLegalFormation(temp_blue)))
			{
				temp_red = InputFormation();
				temp_blue = InputFormation();
			}
			Init(temp_red, temp_blue);
		}

		void EwnState::TakeAction(const EwnAction & action)
		{
			if (action.source != action.dest)
			{
				_piece_flag.reset(_board[action.dest]);
				_board[action.dest] = _board[action.source];
				_board[action.source] = 0;
				_next_player = _next_player == RED ? BLUE : RED;
			}
			_roll_result = action.roll;
		}

		AgentIndex EwnState::GetWinner() const
		{
			if (_board.element(0, 0) > 6)
				return BLUE;
			if (_board.element(g_WIDTH, g_HEIGHT) > 0 && _board.element(g_WIDTH, g_HEIGHT) <= 6)
				return RED;
			if ((_piece_flag & 0x7E).none())
				return BLUE;
			if ((_piece_flag & 0x1F80).none())
				return RED;
			return NO_PLAYER;
		}

		void EwnState::Init(Formation red, Formation blue)
		{
			Coordinate red_coord[6] = { {0,0},{1,0},{2,0},{0,1},{1,1},{0,2} };
			Coordinate blue_coord[6] = { 
				{ g_WIDTH - 1,g_HEIGHT -1 },
				{ g_WIDTH - 2,g_HEIGHT - 1 },
				{ g_WIDTH - 3,g_HEIGHT - 1 },
				{ g_WIDTH - 1,g_HEIGHT - 2 },
				{ g_WIDTH - 2,g_HEIGHT - 2 },
				{ g_WIDTH - 1,g_HEIGHT - 3 }
			};
			for (size_t i = 0; i < 6; i++)
			{
				_board.set_element(red_coord[i], red[i]);
				_board.set_element(blue_coord[i], blue[i] + 6);
			}
		}

		bool EwnState::IsLegalFormation(Formation formation) const
		{
			PieceFlag flag;
			for (size_t i = 0; i< formation.size() && i < 6;i++)
			{
				auto v = formation[i];
				if (v > 0 && v <= 6)
					flag.set(v);
			}
			return flag == PieceFlag(0x7E);
		}

		Formation EwnState::InputFormation() const
		{
			Formation f(6);
			std::string str = console::GetInput("Input Formation >> ");
			if (str.length() == 6)
			{
				for (size_t i = 0; i < 6; i++)
				{
					f[i] = str[i] - '0';
				}
			}
			return f;
		}

		void EwnState::Print() const
		{
			log::ConsoleTable table(g_WIDTH, g_HEIGHT);
			for (auto coord : _board)
			{
				EwnPiece p = piece(coord);
				if (p > 0)
				{
					if (p > 6)//blue
						table[coord] = { ToString((size_t)p - 6), console::BLUE, log::ALIGN_MIDDLE };
					else
						table[coord] = { ToString((size_t)p), console::RED, log::ALIGN_MIDDLE };
				}
			}
			table.print();
		}

		void UpdateState(EwnState & state, const EwnAction & action)
		{
			state.TakeAction(action);
		}

		void MakeAction(const EwnState & state, EwnActionList & action_list)
		{
			if (state.roll_result == 0)
			{
				for (RollResult r = 0; r < 6; r++)
					action_list.push_back({ {0,0},{0,0},r });
				return;
			}
			EwnPlayer player = state.next_player();
			RollResult roll = state.roll_result();
			int distant = player == RED ? 1 : -1;
			Coordinate dir[3] = { { distant, 0 },{ 0,distant },{ distant,distant } };
			if (state.piece_exist(player, roll))
			{
				for (auto d : dir)
				{
					Coordinate dest = 
				}
			}

		}

		EwnPlayer DetemineWinner(const EwnState & state)
		{
			return EwnPlayer();
		}

		EwnPlayer StateToResult(const EwnState & state, AgentIndex winner)
		{
			return EwnPlayer();
		}

		bool AllowUpdateValue(const EwnState & state, EwnPlayer winner)
		{
			return false;
		}

		

		void DefineEwnShell(shell::GameShell& shell)
		{
			auto ewn = shell.CreateShellPage<EwnState>("ewn", Formation{ 1,2,3,4,5,6 }, Formation{ 1,2,3,4,5,6 });
			ewn->AddFunction("print", "print state", [](EwnState& state)->void {state.Print(); });
		}
	}
}
