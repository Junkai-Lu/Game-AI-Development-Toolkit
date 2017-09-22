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

#pragma once

namespace gadt
{
	namespace renju
	{
		struct Location
		{
			int x;
			int y;
		};

		enum MnkGamePlayer : AgentIndex
		{
			WHITE = -1,
			EMPTY = 0,
			BLACK = 1
		};

		struct MnkGameAction
		{
			Location loc;
			MnkGamePlayer piece;
		};

		template<size_t _WIDTH, size_t _HEIGHT>
		class MnkGameStateIter
		{
		private:
			Location _loc;

		public:
			MnkGameStateIter(Location loc):
				_loc(loc)
			{
			}

			bool operator!=(const MnkGameStateIter& iter) const
			{
				if (_loc.x != iter._loc.x || _loc.y != iter._loc.y)
				{
					return true;
				}
				return false;
			}

			void operator++()
			{
				_loc.x++;
				if (_loc.x >= _WIDTH)
				{
					_loc.x = 0;
					_loc.y++;
				}
			}

			Location operator* ()
			{
				return _loc;
			}
		};

		template<size_t _WIDTH, size_t _HEIGHT, size_t _WIN_LENGTH>
		class MnkGameState
		{
		public:
			using Iter = MnkGameStateIter<_WIDTH, _HEIGHT>;

		private:
			MnkGamePlayer _piece[_WIDTH][_HEIGHT];
			AgentIndex _winner;
			AgentIndex _next_player;

		public:

			MnkGameState() :
				_winner(0),
				_next_player(BLACK)
			{
				for (auto loc : *this)
				{
					_piece[loc.x][loc.y] = EMPTY;
				}
			}

			MnkGameState(const MnkGameState& prev_state, const MnkGameAction& action)
			{
				*this = prev_state;
				TakeAction(action);
			}

			inline bool TakeAction(const MnkGameAction& action)
			{
				_piece[action.loc.x][action.loc.y] = action.piece;
				_next_player = -action.piece;
				_winner = JudgeWinnerFromPiece(action.loc);
			}

			bool is_empty()
			{
				for (auto loc : *this)
				{
					if (piece(loc) != EMPTY)
						return false;
				}
				return true;
			}

			bool is_legal_location(const Location& loc)
			{
				return loc.x >= 0 && loc.x < _WIDTH && loc.y >= 0 && loc.y < _HEIGHT;
			}

			inline MnkGamePlayer piece(size_t x, size_t y) const { return _piece[x][y]; }
			inline MnkGamePlayer piece(Location loc) const { return _piece[loc.x][loc.y]; }
			inline AgentIndex winner() const { return _winner; }
			inline AgentIndex next_player() const { return _next_player; }

			AgentIndex JudgeWinnerFromPiece(const Location& loc)
			{
				if (piece(loc) == EMPTY) { return EMPTY; }
				int dir[4][2] = { {0,1},{1,1},{1,0},{1,-1} };
				for (size_t i = 0; i < 4; i++)
				{
					Location check_loc = loc;
					size_t count = 0;
					while ( is_legal_location(check_loc) && piece(chck_loc) == piece(loc))
					{
						check_loc = { check_loc - dir[i][0], check_loc - dir[i][1] };
					}
					while (is_legal_location(check_loc) && piece(chck_loc) == piece(loc))
					{
						check_loc = { check_loc + dir[i][0], check_loc + dir[i][1] };
						count++;
						if (count >= _WIN_LENGTH)
						{
							return piece(loc);
						}
					}
				}
				return EMPTY;
			}

			void Print() const
			{
				table::ConsoleTable tb(_WIDTH, _HEIGHT);
				for (auto loc : *this)
				{
					if (piece(loc) == BLACK)
					{
						tb.cell(loc.x, loc.y) = { "¡ñ" };
					}
					else if (piece(loc) == WHITE)
					{
						tb.cell(loc.x, loc.y) = { "¡ð" };
					}
					else
					{
						if (loc.x == 0)
						{
							if (loc.y == 0)
								tb.cell(loc.x, loc.y) = { "©³" };
							else if (loc.y == _HEIGHT - 1)
								tb.cell(loc.x, loc.y) = { "©»" };
							else
								tb.cell(loc.x, loc.y) = { "©Ç" };
						}
						else if (loc.x == _WIDTH - 1)
						{
							if (loc.y == 0)
								tb.cell(loc.x, loc.y) = { "©·" };
							else if (loc.y == _HEIGHT - 1)
								tb.cell(loc.x, loc.y) = { "©¿" };
							else
								tb.cell(loc.x, loc.y) = { "©Ï" };
						}
						else
						{
							if (loc.y == 0)
								tb.cell(loc.x, loc.y) = { "©×" };
							else if (loc.y == _HEIGHT -1)
								tb.cell(loc.x, loc.y) = { "©ß" };
							else
								tb.cell(loc.x, loc.y) = { "©í" };
						}
					}
				}
				tb.print(false, true);
			}

			inline Iter begin() const
			{
				return Iter({ 0,0 });
			}

			inline Iter end() const
			{
				return Iter({ 0,_HEIGHT });
			}
		};

		template<size_t _WIDTH, size_t _HEIGHT, size_t _WIN_LENGTH>
		class MnkGameActionGenerator
		{
		public:
			using State = MnkGameState<_WIDTH, _HEIGHT, _WIN_LENGTH>;
			using Action = MnkGameAction;

		private:
			const State& _state;

		public:
			std::vector<Action> GetNearbyActions(size_t max_dist)
			{
				int dir[8][2] = { { 0,1 },{ 1,1 },{ 1,0 },{ 1,-1 }, {0, -1},{-1,-1},{-1,0},{-1,1} };
				bool front[_WIDTH][_HEIGHT];
				size_t count = 0;
				for (size_t y = 0; y < _HEIGHT; y++)
				{
					for (size_t x = 0; x < _WIDTH; x++)
					{
						if (_state.piece(x, y) == _state.next_player())
						{
							for (size_t n = 0; n < 8; n++)
							{
								int new_x = x + dir[n][0];
								int new_t = y + dir[n][1];
								if (_state.is_legal_location({ new_x,new_y }))
								{
									if (_state.piece(new_x, new_y) == EMPTY)
									{
										front[new_x][new_y] = true;
										count++;
									}
								}
							}
						}
						else
						{
							front[x][y] = false;
						}
					}
				}
				for (size_t i = 1; i < max_dist; i++)
				{
					for (size_t y = 0; y < _HEIGHT; y++)
					{
						for (size_t x = 0; x < _WIDTH; x++)
						{
							if (front[x][y] == true)
							{
								for (size_t n = 0; n < 8; n++)
								{
									int new_x = x + dir[n][0];
									int new_t = y + dir[n][1];
									if (_state.is_legal_location({ new_x,new_y }))
									{
										if (_state.piece(new_x, new_y) == EMPTY && front[new_x][new_y] == false)
										{
											front[new_x][new_y] = true;
											count++;
										}
									}
								}
							}
						}
					}
				}

				std::vector<Action> temp(count);
				

				return temp;
			}

			std::vector<Action> GetAllActions()
			{
				for (size_t y = 0; y < _HEIGHT; y++)
				{
					for (size_t x = 0; x < _WIDTH; x++)
					{
						if (_state.piece(x, y) == _state.next_player())
						{
							for (size_t n = 0; n < 8; n++)
							{
								int new_x = x + dir[n][0];
								int new_t = y + dir[n][1];
								if (_state.is_legal_location({ new_x,new_y }))
									if (_state.piece(new_x, new_y) == EMPTY)
										front[new_x][new_y] = true;
							}
						}
						else
						{
							front[x][y] = false;
						}
					}
				}

				std::vector<Action> temp(count);


				return temp;
			}
		};

		using RenjuState = MnkGameState<9, 9, 5>;
		using RenjuAction = MnkGameAction;
		using RenjuActionGenerator = MnkGameActionGenerator<9, 9, 5>;
	}
}