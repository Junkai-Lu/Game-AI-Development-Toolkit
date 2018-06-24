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
			UPoint point;
			AgentIndex piece;

			template<typename T>
			MnkGameAction(UPoint _point, T _piece):
				point(_point),
				piece((AgentIndex)_piece)
			{
			}

			std::string to_string() const
			{
				std::stringstream ss;
				ss << "{ " << point.to_string() << " , " << (int)piece << " }";
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
			stl::StaticMatrix<AgentIndex, _WIDTH, _HEIGHT> _piece;
			stl::StaticMatrix<bool, _WIDTH, _HEIGHT> _action_range;
			AgentIndex _winner;
			AgentIndex _next_player;
			size_t _piece_count;

		private:
			void SetActionRange(const UPoint& point)
			{
				for (int i = 1; i <= _ACTION_RANGE; i++)
				{
					Point dir[8] = { {0,1}, { 1,1 }, { 1,0 }, { 1,-1 }, { 0, -1 }, { -1,-1 }, { -1,0 }, { -1,1 }};
					for (Point d : dir)
					{
						UPoint new_point = (point.to_signed() + (d * i)).to_unsigned();
						if(_piece.is_legal_point(new_point))
							if (_piece[new_point] == 0)
								_action_range[new_point] = true;
					}
				}
			}

		public:
			inline constexpr size_t width() const { return _WIDTH; }
			inline constexpr size_t height() const { return _HEIGHT; }
			inline Iter begin() const { return _piece.begin(); }
			inline Iter end() const { return _piece.end(); }
			inline AgentIndex piece(UPoint point) const { return _piece.element(point); }
			inline AgentIndex winner() const { return _winner; }
			inline AgentIndex next_player() const { return _next_player; }
			inline bool is_empty()
			{
				for (auto point : _piece)
				{
					if (piece(point) != 0)
						return false;
				}
				return true;
			}
			inline bool in_action_range(const UPoint& point) const
			{
				return _action_range.element(point);
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
				_piece.set_element(action.piece, action.point);
				_next_player = AgentIndex(-action.piece);
				_winner = JudgeWinnerFromPiece(action.point);
				_action_range.set_element(false, action.point);
				_piece_count++;
				SetActionRange(action.point);
			}

			AgentIndex JudgeWinnerFromPiece(const UPoint& point)
			{
				if (piece(point) == 0) { return 0; }
				Point dir[4] = { { 0,1 },{ 1,1 },{ 1,0 },{ 1,-1 } };
				for (size_t i = 0; i < 4; i++)
				{
					UPoint check_point = point;
					size_t count = 0;
					while (_piece.is_legal_point(check_point) && piece(check_point) == piece(point))
					{
						check_point = (check_point.to_signed() - dir[i]).to_unsigned();
					}
					check_point = (check_point.to_signed() + dir[i]).to_unsigned();
					while (_piece.is_legal_point(check_point) && piece(check_point) == piece(point))
					{
						check_point = (check_point.to_signed() + dir[i]).to_unsigned();
						count++;
						if (count >= _WIN_LENGTH)
						{
							return piece(point);
						}
					}
				}
				return 0;
			}

			inline bool IsLegalAction(const MnkGameAction& action)
			{
				return _piece.is_legal_point(action.point);
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
				for (UPoint point : _state)
					if (_state.in_action_range(point))
						actions.push_back({ point, player });
				if (actions.size() == 0)
					actions.push_back({ {_WIDTH / 2, _HEIGHT / 2}, player });
				return actions;
			}

			ActionList GetAllActions(AgentIndex player)
			{
				ActionList actions;
				for (UPoint point : _state)
					if (_state.piece(point) == 0)
						actions.push_back({ point, player });
				return actions;
			}
		};
	}
}