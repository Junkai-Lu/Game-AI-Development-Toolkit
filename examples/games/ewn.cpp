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
			_board(g_EMPTY),
			_piece_flag(0xFFF),
			_next_player(RED),
			_roll_result(g_EMPTY)
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
			_board(g_EMPTY),
			_piece_flag(0xFFF),
			_next_player(RED),
			_roll_result(g_EMPTY)
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
				EwnPiece piece = _board[action.source];
				if(_board[action.dest] >= 0)
					_piece_flag.reset(_board[action.dest]);
				_board[action.dest] = piece;
				_piece_coord[piece] = action.dest;
				_board[action.source] = g_EMPTY;
				_next_player = _next_player == RED ? BLUE : RED;
			}
			_roll_result = action.roll;
		}

		EwnPlayer EwnState::GetWinner() const
		{
			if (_board.element(0, 0) >= 6)
				return BLUE;
			if (_board.element(g_WIDTH - 1, g_HEIGHT -1 ) >= 0 && _board.element(g_WIDTH - 1, g_HEIGHT - 1 ) < 6)
				return RED;
			if ((_piece_flag & 0x3F).none())
				return BLUE;
			if ((_piece_flag & 0xFC0).none())
				return RED;
			return NO_PLAYER;
		}

		void EwnState::Init(Formation red, Formation blue)
		{
			UnsignedCoordinate red_coord[6] = { {0,0},{1,0},{2,0},{0,1},{1,1},{0,2} };
			UnsignedCoordinate blue_coord[6] = { 
				{ g_WIDTH - 1,g_HEIGHT -1 },
				{ g_WIDTH - 2,g_HEIGHT - 1 },
				{ g_WIDTH - 3,g_HEIGHT - 1 },
				{ g_WIDTH - 1,g_HEIGHT - 2 },
				{ g_WIDTH - 2,g_HEIGHT - 2 },
				{ g_WIDTH - 1,g_HEIGHT - 3 }
			};
			for (size_t i = 0; i < 6; i++)
			{
				_board.set_element(red[i] - 1, red_coord[i]);
				_board.set_element(blue[i] + 5, blue_coord[i]);
				_piece_coord[i] = red_coord[i];
				_piece_coord[i + 6] = blue_coord[i];
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
			console::Table table(g_WIDTH, g_HEIGHT);
			for (auto coord : _board)
			{
				EwnPiece p = piece(coord);
				if (p >= 0)
				{
					if (p >= 6)//blue
						table[coord] = { ToString((size_t)p - 5), console::COLOR_BLUE, console::TABLE_ALIGN_MIDDLE };
					else
						table[coord] = { ToString((size_t)p + 1), console::COLOR_RED, console::TABLE_ALIGN_MIDDLE };
				}
			}
			table.Print(console::TABLE_FRAME_ENABLE, console::TABLE_INDEX_FROM_ZERO);
			std::cout << "    >> Roll = " << (int)roll_result() + 1 << std::endl;
			std::cout << "    >> Piece Flag = " << _piece_flag.to_string().substr(52,12) << std::endl;
		}

		void EwnActionGenerator::AddActionForPiece(EwnActionList& actions, EwnPlayer player, RollResult roll, bool enable_herustic) const
		{
			int distant = player == RED ? 1 : -1;
			Coordinate target = ((player == RED) ? Coordinate{ g_WIDTH - 1, g_HEIGHT - 1 } : Coordinate{ 0, 0 });
			Coordinate dir[3] = { { distant, 0 },{ 0,distant },{ distant,distant } };
			Coordinate source_coord = _state.piece_coord(player, roll).to_signed();
			size_t source_dis = enable_herustic ? func::GetManhattanDistance(source_coord, target) : 0;
			for (auto d : dir)
			{
				Coordinate dest_coord = source_coord + d;
				size_t dest_dis = enable_herustic ? func::GetManhattanDistance(dest_coord, target) : 0;
				if (_state.is_legal_coord(dest_coord.to_unsigned()) && (!enable_herustic || dest_dis < source_dis))
					actions.push_back({ source_coord.to_unsigned(), dest_coord.to_unsigned(), g_EMPTY });
			}
		}

		EwnActionList EwnActionGenerator::GetActions(bool enable_herustic) const
		{
			EwnActionList actions;
			if (_state.roll_result() == g_EMPTY)
			{
				for (RollResult r = 0; r < 6; r++)
					actions.push_back({ { 0,0 },{ 0,0 },r });
				return actions;
			}
			EwnPlayer player = _state.next_player();
			RollResult roll = _state.roll_result();
			if (_state.piece_exist(player, roll))
			{
				AddActionForPiece(actions, player, roll, enable_herustic);
			}
			else
			{
				RollResult neigh[2] = { _state.GetNeighbourPiece(player, roll, -1),_state.GetNeighbourPiece(player, roll, 1) };
				for (auto n : neigh)
					if (n >= 0 && n < 6)
						AddActionForPiece(actions, player, n, enable_herustic);
			}
			return actions;
		}

		void UpdateState(EwnState & state, const EwnAction & action)
		{
			state.TakeAction(action);
		}

		void MakeAction(const EwnState & state, EwnActionList & action_list)
		{
			action_list = EwnActionGenerator(state).GetAllActions();
		}

		EwnPlayer DetemineWinner(const EwnState & state)
		{
			return state.GetWinner();
		}

		EwnPlayer StateToResult(const EwnState & state, EwnPlayer winner)
		{
			return winner;
		}

		bool AllowUpdateValue(const EwnState & state, EwnPlayer winner)
		{
			return false;
		}

		void DefineEwnShell(shell::GameShell& shell)
		{
			auto ewn = shell.CreateShellPage<EwnState>("ewn", Formation{ 1,2,3,4,5,6 }, Formation{ 1,2,3,4,5,6 });

			ewn->AddFunction("print", "print state", [](EwnState& state)->void {state.Print(); });

			ewn->AddFunction("actions", "show actions", [](EwnState& state)->void{
				state.Print();
				EwnActionGenerator generator(state);
				auto actions = generator.GetAllActions();
				size_t index = 0;
				for (auto act : actions)
					std::cout << "    " << index ++ <<": "<< act.to_string() << std::endl;
				size_t input = console::GetInput<size_t>();
				if (input < actions.size())
				{
					std::cout << "take action " << actions[input].to_string() << std::endl;
					UpdateState(state, actions[input]);
					state.Print();
				}
			});

			ewn->AddFunction("random", "random action", [](EwnState& state)->void{
				auto actions = EwnActionGenerator(state).GetAllActions();
				auto act = func::GetRandomElement(actions);
				UpdateState(state, act);
				std::cout << "take action = " << act.to_string() << std::endl;
				state.Print();
			});

			ewn->AddFunction("winner", "get winner", [](EwnState& state)->void{
				auto winner = DetemineWinner(state);
				switch (winner)
				{
				case gadt::ewn::NO_PLAYER:
					std::cout << "NO PLAYER" << std::endl;
					break;
				case gadt::ewn::RED:
					console::Cprintf("RED\n", console::COLOR_RED);
					break;
				case gadt::ewn::BLUE:
					console::Cprintf("BLUE\n", console::COLOR_BLUE);
					break;
				default:
					break;
				}
			});
		}
		
}
}
