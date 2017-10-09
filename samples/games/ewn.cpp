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
		}

		EwnState::EwnState(Formation red, Formation blue) :
			_board(0),
			_piece_flag(0),
			_next_player(RED),
			_roll_result(0)
		{
			if (IsLegalFormation(red) && IsLegalFormation(blue))
			{

			}
		}

		void EwnState::TakeAction(const EwnAction & action)
		{
		}

		AgentIndex EwnState::GetWinner() const
		{
			return 0;
		}

		void EwnState::Init(Formation red, Formation blue)
		{
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

		void EwnState::Print() const
		{
			log::ConsoleTable table(g_WIDTH, g_HEIGHT);
			for (auto coord : _board)
			{
				EwnPiece p = piece(coord);
			
			}
		}

		void UpdateState(EwnState & state, const EwnAction & action)
		{
			state.TakeAction(action);
		}

		void DefineEwnShell(shell::GameShell& shell, std::string page_name)
		{

		}
	}
}
