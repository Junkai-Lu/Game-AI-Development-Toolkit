#include "gadtlib.h"
#include "gadt_container.hpp"

#pragma once

namespace gadt
{
	namespace player
	{
		//PlayerIndex allows to define a index start from any number and with any length.
		template< typename IndexType, IndexType BEGIN_INDEX, IndexType COUNT, typename std::enable_if<std::is_integral<IndexType>::value, int>::type = 0 >	
		class PlayerRange
		{
		private:

			IndexType _index;

		public:

			//default construstor.
			PlayerRange()
			{
			}

			//get next index. 
			inline IndexType get_next(IndexType agent_index) const
			{
				constexpr int end = COUNT != 0 ? BEGIN_INDEX + (int)COUNT : BEGIN_INDEX + 1;
				return agent_index + 1 < end ? agent_index + 1 : BEGIN_INDEX;
			}

			//get prev index.
			inline IndexType get_prev(IndexType agent_index) const
			{
				constexpr int end = COUNT != 0 ? BEGIN_INDEX + (int)COUNT : BEGIN_INDEX + 1;
				return agent_index - 1 < BEGIN_INDEX ? end - 1 : agent_index - 1;
			}

			//get index after jump.
			inline IndexType get_jump(IndexType agent_index, size_t jump_value) const
			{
				constexpr int end = (COUNT != 0 ? BEGIN_INDEX + (int)COUNT : BEGIN_INDEX + 1);
				jump_value = jump_value % COUNT;
				if (agent_index + jump_value >= end)
				{
					return agent_index - IndexType(COUNT - jump_value);
				}
				return agent_index + (IndexType)jump_value;
			}

			//move to next index.
			inline void to_next()
			{
				_index = get_next(_index);
			}

			//move to previous index
			inline void to_prev()
			{
				_index = get_prev(_index);
			}

			//jump appointed index.
			inline void jump(size_t jump_value)
			{
				_index = get_jump(_index);
			}

			//get current index
			inline IndexType current() const
			{
				return _index;
			}

			//get begin index
			constexpr inline IndexType begin_index() const
			{
				return BEGIN_INDEX;
			}

			//get count of players.
			constexpr inline size_t count() const
			{
				return COUNT;
			}
		};
	}

	namespace game
	{
		//define action of mnk game.
		struct MnkGameAction
		{
			Coordinate coord;
			AgentIndex piece;

			template<typename T>
			MnkGameAction(Coordinate _coord, T _piece):
				coord(_coord),
				piece((AgentIndex)_piece)
			{
			}

			std::string to_string() const
			{
				std::stringstream ss;
				ss << "{ " << coord.to_string() << " , " << (int)piece << " }";
				return ss.str();
			}
		};

		//game state of mnk game.
		template<size_t _WIDTH, size_t _HEIGHT, size_t _WIN_LENGTH, size_t _ACTION_RANGE = 2>
		class MnkGameState
		{
		public:
			using Iter = stl::MatrixIter;

		private:
			stl::RectangularArray<AgentIndex, _WIDTH, _HEIGHT> _piece;
			stl::RectangularArray<bool, _WIDTH, _HEIGHT> _action_range;
			AgentIndex _winner;
			AgentIndex _next_player;
			size_t _piece_count;

		private:
			void SetActionRange(const Coordinate& coord)
			{
				for (int i = 1; i <= _ACTION_RANGE; i++)
				{
					Coordinate dir[8] = { {0,1}, { 1,1 }, { 1,0 }, { 1,-1 }, { 0, -1 }, { -1,-1 }, { -1,0 }, { -1,1 }};
					for (Coordinate d : dir)
					{
						Coordinate new_coord = coord + (d * i);
						if(_piece.is_legal_coordinate(new_coord))
							if (_piece[new_coord] == 0)
								_action_range[new_coord] = true;
					}
				}
			}

		public:
			inline constexpr size_t width() const { return _WIDTH; }
			inline constexpr size_t height() const { return _HEIGHT; }
			inline Iter begin() const { return _piece.begin(); }
			inline Iter end() const { return _piece.end(); }
			inline AgentIndex piece(Coordinate coord) const { return _piece.element(coord); }
			inline AgentIndex winner() const { return _winner; }
			inline AgentIndex next_player() const { return _next_player; }
			inline bool is_empty()
			{
				for (auto coord : _piece)
				{
					if (piece(coord) != 0)
						return false;
				}
				return true;
			}
			inline bool in_action_range(const Coordinate& coord) const
			{
				return _action_range.element(coord);
			}
			inline bool is_draw() const
			{
				return _piece_count == _WIDTH * _HEIGHT;
			}

		public:

			MnkGameState(AgentIndex first_player) :
				_piece(0),
				_action_range(false),
				_winner(0),
				_next_player(first_player),
				_piece_count(0)
			{
			}

			MnkGameState(const MnkGameState& prev_state, const MnkGameAction& action)
			{
				*this = prev_state;
				TakeAction(action);
			}

			void TakeAction(const MnkGameAction& action)
			{
				_piece.set_element(action.coord, action.piece);
				_next_player = AgentIndex(-action.piece);
				_winner = JudgeWinnerFromPiece(action.coord);
				_action_range.set_element(action.coord, false);
				_piece_count++;
				SetActionRange(action.coord);
			}

			AgentIndex JudgeWinnerFromPiece(const Coordinate& coord)
			{
				if (piece(coord) == 0) { return 0; }
				Coordinate dir[4] = { { 0,1 },{ 1,1 },{ 1,0 },{ 1,-1 } };
				for (size_t i = 0; i < 4; i++)
				{
					Coordinate check_coord = coord;
					size_t count = 0;
					while (_piece.is_legal_coordinate(check_coord) && piece(check_coord) == piece(coord))
					{
						check_coord -= dir[i];
					}
					check_coord += dir[i];
					while (_piece.is_legal_coordinate(check_coord) && piece(check_coord) == piece(coord))
					{
						check_coord += dir[i];
						count++;
						if (count >= _WIN_LENGTH)
						{
							return piece(coord);
						}
					}
				}
				return 0;
			}

			inline bool IsLegalAction(const MnkGameAction& action)
			{
				return _piece.is_legal_coordinate(action.coord);
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

			ActionList GetNearbyActions(AgentIndex player)
			{
				std::vector<Action> actions;
				for (Coordinate coord : _state)
					if (_state.in_action_range(coord))
						actions.push_back({ coord, player });
				if (actions.size() == 0)
					actions.push_back({ {_WIDTH / 2, _HEIGHT / 2}, player });
				return actions;
			}

			ActionList GetAllActions(AgentIndex player)
			{
				ActionList actions;
				for (Coordinate coord : _state)
					if (_state.piece(coord) == 0)
						actions.push_back({ coord, player });
				return actions;
			}
		};
	}
}