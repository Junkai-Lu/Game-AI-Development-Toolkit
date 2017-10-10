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
	namespace ewn
	{
		constexpr const size_t g_WIDTH = 5;
		constexpr const size_t g_HEIGHT = 5;

		//piece. -1 is empty, 0~5 are red pieces, 6~11 are blue pieces.
		using EwnPiece = int8_t;
		using RollResult = uint8_t;
		using Formation = std::vector<RollResult>;

		//piece types in board.
		enum EwnPlayer : AgentIndex
		{
			NO_PLAYER = 0,
			RED = 1,
			BLUE = 2
		};

		struct EwnAction
		{
			Coordinate source;
			Coordinate dest;
			RollResult roll;

			std::string to_string() const
			{
				std::stringstream ss;
				ss << "from " << source.to_string() << " to " << dest.to_string();
				return ss.str();
			}
		};

		using EwnActionList = std::vector<EwnAction>;

		//state class
		class EwnState
		{
		private:
			using EwnBoard = stl::MatrixArray<EwnPiece, g_WIDTH, g_HEIGHT>;
			using PieceFlag = bitboard::BitBoard64;

		private:
			EwnBoard _board;
			Coordinate _piece_coord[12];
			PieceFlag _piece_flag;
			EwnPlayer _next_player;
			RollResult _roll_result;

		private:
			void Init(Formation red, Formation blue);

			bool IsLegalFormation(Formation formation) const;

			Formation InputFormation() const;

		public:
			
			EwnState();

			EwnState(Formation red_formation , Formation blue_formation);

			void TakeAction(const EwnAction& action);

			AgentIndex GetWinner() const;

			void Print() const;

			RollResult GetNeighbourPiece(EwnPlayer player, RollResult roll, int step)
			{
				int temp = roll + step;
				while (temp > 0 && temp < 7 && !piece_exist(player, roll))
					temp += step;
				return (RollResult)temp;
			}

			inline EwnPiece piece(Coordinate coord) const { return _board.element(coord); }
			inline bool piece_exist(size_t index) const { return _piece_flag[index]; }
			inline EwnPlayer next_player() const { return _next_player; }
			inline RollResult roll_result() const { return _roll_result; }
			inline bool get_player(EwnPiece piece) const 
			{
				if (piece == 0 || piece > 12)
					return NO_PLAYER;
				return piece <= 6 ? RED : BLUE;
			}
			inline bool piece_exist(AgentIndex player, RollResult roll) const { return _piece_flag[((player - 1) * 6 + roll)]; }
		};

		void UpdateState(EwnState& state, const EwnAction& action);

		void MakeAction(const EwnState& state, EwnActionList& action_list);

		EwnPlayer DetemineWinner(const EwnState& state);

		EwnPlayer StateToResult(const EwnState& state, AgentIndex winner);

		bool AllowUpdateValue(const EwnState& state, EwnPlayer winner);

		void DefineEwnShell(shell::GameShell& shell);


	}
}
