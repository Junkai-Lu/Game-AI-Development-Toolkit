#include "gadtlib.h"

#pragma once

namespace gadt
{
	namespace game
	{
		using Location = Coordinate;

		//define game player of mnk game.
		enum MnkGamePlayer : AgentIndex
		{
			WHITE = -1,
			EMPTY = 0,
			BLACK = 1,
			FLAG = 2
		};

		//define action of mnk game.
		struct MnkGameAction
		{
			Location loc;
			MnkGamePlayer piece;

			template<typename T>
			MnkGameAction(Location _loc, T _piece):
				loc(_loc),
				piece((MnkGamePlayer)_piece)
			{
			}
		};

		//iterator of mnk game state.
		template<size_t _WIDTH, size_t _HEIGHT>
		class MnkGameStateIter
		{
		private:
			Location _loc;

		public:
			MnkGameStateIter(Location loc) :
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

		//game state of mnk game.
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

			inline void TakeAction(const MnkGameAction& action)
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

			bool is_legal_location(const Location& loc) const
			{
				return loc.x >= 0 && loc.x < _WIDTH && loc.y >= 0 && loc.y < _HEIGHT;
			}

			bool is_legal_action(const MnkGameAction& action)
			{
				return is_legal_location(action.loc) && WHITE <= action.piece && BLACK >= action.piece;
			}

			inline MnkGamePlayer piece(size_t x, size_t y) const { return _piece[x][y]; }
			inline MnkGamePlayer piece(Location loc) const { return _piece[loc.x][loc.y]; }
			inline AgentIndex winner() const { return _winner; }
			inline AgentIndex next_player() const { return _next_player; }

			AgentIndex JudgeWinnerFromPiece(const Location& loc)
			{
				if (piece(loc) == EMPTY) { return EMPTY; }
				Location dir[4] = { { 0,1 },{ 1,1 },{ 1,0 },{ 1,-1 } };
				for (size_t i = 0; i < 4; i++)
				{
					Location check_loc = loc;
					size_t count = 0;
					while (is_legal_location(check_loc) && piece(check_loc) == piece(loc))
					{
						check_loc -= dir[i];
					}
					check_loc += dir[i];
					while (is_legal_location(check_loc) && piece(check_loc) == piece(loc))
					{
						check_loc += dir[i];
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
				std::stringstream ss;
				ss << "player:" << (int)_next_player << " winner:" << (int)_winner;
				tb.enable_title({ ss.str() });
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
					else if (piece(loc) == FLAG)
					{
						tb.cell(loc.x, loc.y) = { "¡ï" };
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
							else if (loc.y == _HEIGHT - 1)
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

			inline size_t width() const
			{
				return _WIDTH;
			}

			inline size_t height() const
			{
				return _HEIGHT;
			}
		};

		//action generator of mnk game.
		template<size_t _WIDTH, size_t _HEIGHT, size_t _WIN_LENGTH>
		class MnkGameActionGenerator
		{
		public:
			using State = MnkGameState<_WIDTH, _HEIGHT, _WIN_LENGTH>;
			using Action = MnkGameAction;
			using ActionList = std::vector<Action>;

		private:
			const State& _state;

		public:
			MnkGameActionGenerator(const State& state):
				_state(state)
			{
			}

			ActionList GetNearbyActions(size_t max_dist)
			{
				Location dir[8] = { { 0,1 },{ 1,1 },{ 1,0 },{ 1,-1 },{ 0, -1 },{ -1,-1 },{ -1,0 },{ -1,1 } };
				bool front[_WIDTH][_HEIGHT];
				for (auto loc : _state)
					front[loc.x][loc.y] = false;
				for (size_t dist = 0; dist < max_dist; dist++)
				{
					for(auto loc: _state)
					{
						if ((dist != 0 && front[loc.x][loc.y] == true) || (dist == 0 && _state.piece(loc) != EMPTY))
						{
							for (size_t n = 0; n < 8; n++)
							{
								Location new_loc = loc + dir[n];
								if (_state.is_legal_location(new_loc))
								{
									if (_state.piece(new_loc) == EMPTY)
									{
										front[new_loc.x][new_loc.y] = true;
									}
								}
							}
						}
					}
				}
				std::vector<Action> temp;
				for (auto loc : _state)
				{
					if (front[loc.x][loc.y])
						temp.push_back({ loc,_state.next_player() });
				}
				return temp;
			}

			ActionList GetAllActions()
			{
				ActionList temp;
				for (Location loc : _state)
					if (_state.piece(loc) == EMPTY)
						temp.push_back(Action(loc, _state.next_player()));
				return temp;
			}

			void Print(ActionList action_list)
			{
				State temp = _state;
				for (auto act : action_list)
				{
					temp.TakeAction({ act.loc , FLAG });
				}
				temp.Print();
			}
		};
	}
}
