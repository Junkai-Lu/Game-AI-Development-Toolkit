/*
* General MCTS Lib
*/

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
#include "gadtlib.hpp"

#pragma once

namespace gadt
{
	//State Info
	template <
		typename t_State,
		typename t_Action,
		void(*make_action_func)(const t_State&, std::vector<t_Action>&),
		int(*winner_determind_func)(const t_State&),
		bool debug_model
	>
	class StateInfo final
	{
	private:
		t_State _state;
		std::vector<t_Action> _action_vec;
		size_t _next_action_index;
		int _winner;

	public:
		StateInfo(const t_State& state) :
			_state(state),
			_next_action_index(0),
			_winner(winner_determind_func(state))
		{
			if (_winner < 0)//no winner.
			{
				make_action_func(state, _action_vec);
				if (debug_model)
				{
					GADT_WARNING_CHECK(_action_vec.size() == 0, "wrong make action func");
				}
			}
		}

		inline const t_Action& next_action()
		{
			return _action_vec[_next_action_index++];
		}

		inline const t_Action& random_action() const
		{
			return _action_vec[rand() % _action_vec.size()];
		}

		inline const t_Action& action(size_t index) const
		{
			return _action_vec[index];
		}

		inline bool exist_unactivated_action() const
		{
			return _next_action_index != _action_vec.size();
		}

		inline int winner() const
		{
			return _winner;
		}

		inline const t_State& state() const
		{
			return _state;
		}

		inline bool is_end_state() const
		{
			return _winner >= 0;
		}
	};

	//MCTS result.
	template <
		typename t_State
	>
	class MctsResult final
	{
	private:
		t_State _result_state;
		int _result_winner;

	public:
		MctsResult(t_State state, int winner) :
			_result_state(state),
			_result_winner(winner)
		{
		}

		const t_State& state()
		{
			return _result_state;
		}

		int winner()
		{
			return _result_winner;
		}
	};


	//MCTS node.
	template <
		typename t_State,
		typename t_Action,
		void(*state_generate_func)(t_State&, int&, const t_Action&),
		void(*make_action_func)(const t_State&, std::vector<t_Action>&),
		int(*winner_determind_func)(const t_State&),
		bool debug_model
	>
	class MctsNode final
	{
		typedef StateInfo<t_State, t_Action, make_action_func, winner_determind_func, debug_model> t_StateInfo;
		typedef MctsResult<t_State> t_MctsResult;

		//Custom Function.
		typedef std::function<void(t_State&, int&, const t_Action&)> FuncStateGenerate;
		typedef std::function<void(const t_State&, std::vector<t_Action>&)> FuncMakeAction;
		typedef std::function<int(const t_State&, int)> FuncWinnerDetermind;

		typedef std::function<void(const t_State&, int, size_t&, int)> FuncBackpropagate;
		typedef std::function<double(size_t, size_t, size_t, size_t)> FuncUcbForParent;
		typedef std::function<void(const t_State&, int, t_State&, int&)> FuncDefaultPolicy;
		typedef std::function<bool(const t_State&, size_t, size_t)> FuncNodeExtend;

	private:
		t_StateInfo* _state_info;
		size_t _visited_time;
		size_t _win_time;
		int _player_index;
		std::vector<MctsNode*> _child_node;

	private:

		//clear state info of this node. NOTICE: this function should be executed after all child node of this node had been created.
		inline void clear_state_info()
		{
			delete _state_info;
			_state_info = nullptr;
		}

		//get whether the state info exist or had been cleared.
		inline bool exist_state_info()
		{
			return _state_info != nullptr;
		}

		//default policy. return a temp object to 
		const t_MctsResult* DefaultPolicy(FuncDefaultPolicy default_policy_func)
		{
			t_State result_state = _state_info->state();
			int result_winner = _state_info->winner();
			default_policy_func(_state_info->state(), result_state, result_winner);
			return new t_MctsResult(result_state, result_winner);
		}

	public:
		//create root node.
		MctsNode(t_State state, int player_index) :
			_state_info(new t_StateInfo(state)),
			_player_index(player_index),
			_visited_time(1),
			_win_time(0)
		{
		}

		//copy constructor function.
		MctsNode(const MctsNode& target) :
			_state_info(nullptr),
			_visited_time(target._visited_time),
			_win_time(target._win_time),
			_player_index(target._player_index)
		{
			if (target._state_info != nullptr)
			{
				_state_info = new t_StateInfo(*target._state_info);
			}
		}

		~MctsNode()
		{
			if (_state_info != nullptr)
			{
				delete _state_info;
			}
			if (_child_node.size() != 0)
			{
				for (auto child : _child_node)
				{
					delete child;
				}
			}
		}

		//get the best child index that have best ucb value.
		size_t GetBestChild(FuncUcbForParent ucb_for_parent_func)
		{
			double best_ucb = 0;
			size_t best_child = 0;
			for (size_t i = 0; i < _child_node.size(); i++)
			{
				double ucb = ucb_for_parent_func(_child_node[i]->_visited_time, _child_node[i]->_win_time, this->_visited_time, this->_win_time);
				if (ucb > best_ucb)
				{
					best_ucb = ucb;
					best_child = i;
				}
			}
			return best_child;
		}

		//extend node. do action according to the state of node.
		t_MctsResult* Extend(
			FuncBackpropagate back_propagate_func,
			FuncUcbForParent ucb_for_parent_func,
			FuncDefaultPolicy default_policy_func,
			FuncNodeExtend node_extend_func,
			bool auto_clear_state_info
			)
		{
			//increase the visited time of this node.
			_visited_time++;

			//check winner -> check unactivated action -> add new node.
			if (_state_info != nullptr)
			{
				if (_state_info->is_end_state())
				{
					//return current state.
					return new t_MctsResult(_state_info->state(), _state_info->winner());
				}

				if (_state_info->exist_unactivated_action())
				{
					if (_child_node.size() == 0)
					{
						if (!node_extend_func(_state_info->state(), _visited_time, _win_time))
						{
							//do not extend, return default policy of current node;
							t_State result_state = _state_info->state();
							int result_winner = 0;
							default_policy_func(_state_info->state(), this->_player_index, result_state, result_winner);
							back_propagate_func(result_state, result_winner, this->_win_time, this->_player_index);
							return new t_MctsResult(result_state, result_winner);
						}
					}
					//generate new child.
					t_State child_state = this->_state_info->state();
					int child_player = this->_player_index;
					state_generate_func(child_state, child_player, this->_state_info->next_action());
					MctsNode* child = new MctsNode(child_state, child_player);
					_child_node.push_back(child);

					if (auto_clear_state_info)
					{
						if (this->state_info()->exist_unactivated_action() == false)
						{
							this->clear_state_info();
						}
					}

					if (child->state_info()->is_end_state())
					{
						back_propagate_func(child->state_info()->state(), child->state_info()->winner(), child->_win_time, child->player_index());
						back_propagate_func(child->state_info()->state(), child->state_info()->winner(), this->_win_time, this->player_index());
						return new t_MctsResult(child->state_info()->state(), child->state_info()->winner());
					}
					else
					{
						//do default policy in child node.
						t_State result_state = child->state_info()->state();
						int result_winner = 0;
						default_policy_func(child->state_info()->state(), child->player_index(), result_state, result_winner);
						back_propagate_func(result_state, result_winner, child->_win_time, child->player_index());
						back_propagate_func(result_state, result_winner, this->_win_time, this->player_index());
						return new t_MctsResult(result_state, result_winner);
					}
				}
			}

			//do not return. find the child with best ucb value;
			size_t best_child = GetBestChild(ucb_for_parent_func);
			t_MctsResult* result = this->_child_node[best_child]->Extend(
				back_propagate_func,
				ucb_for_parent_func,
				default_policy_func,
				node_extend_func,
				auto_clear_state_info
				);
			back_propagate_func(result->state(), result->winner(), this->_win_time, this->player_index());
			return result;
		}

		size_t TreeSizeAsRoot()
		{
			size_t s = 1;//node itself
			for (auto child : child_node_ref())
			{
				s += child->TreeSizeAsRoot();
			}
			return s;
		}

		//get the state info as a const format.
		inline const t_StateInfo* const state_info() const
		{
			return _state_info;
		}
		inline int player_index() const
		{
			return _player_index;
		}
		inline size_t visited_time() const
		{
			return _visited_time;
		}
		inline size_t win_time() const
		{
			return _win_time;
		}
		inline void add_win_time(size_t t)
		{
			_win_time += t;
		}
		inline double exception()
		{
			return (double)_win_time / (double)_visited_time;
		}
		inline std::vector<MctsNode*> child_node_ref()
		{
			return _child_node;
		}
	};

	//MCTS Search.
	template <
		typename t_State,
		typename t_Action,
		void(*state_generate_func)(t_State&, int&, const t_Action&),
		void(*make_action_func)(const t_State&, std::vector<t_Action>&),
		int(*winner_determind_func)(const t_State&),
		bool debug_model = false
	>
	class MctsSearch final
	{
		//Members
		typedef StateInfo<t_State, t_Action, make_action_func, winner_determind_func, debug_model> t_StateInfo;
		typedef MctsNode<t_State, t_Action, state_generate_func, make_action_func, winner_determind_func, debug_model> t_MctsNode;
		typedef MctsResult<t_State> t_MctsResult;

		//Custom Function.
		typedef std::function<void(t_State&, int&, const t_Action&)> FuncStateGenerate;
		typedef std::function<void(const t_State&, std::vector<t_Action>&)> FuncMakeAction;
		typedef std::function<int(const t_State&, int)> FuncWinnerDetermind;

		typedef std::function<void(const t_State&, int, size_t&, int)> FuncBackpropagate;
		typedef std::function<double(size_t, size_t, size_t, size_t)> FuncUcbForParent;
		typedef std::function<void(const t_State&, int, t_State&, int&)> FuncDefaultPolicy;
		typedef std::function<bool(const t_State&, size_t, size_t)> FuncNodeExtend;
		typedef std::function<void(t_State&)> FuncShowState;
		typedef std::function<void(t_Action&)> FuncShowAction;

	private:
		//custom function.
		FuncBackpropagate _func_back_propagate;
		FuncUcbForParent _func_ucb_for_parent;
		FuncDefaultPolicy _func_default_policy;
		FuncNodeExtend _func_node_extend;

		//setting
		bool _auto_clear_state_info;			//default is true.

	private:
		void FuncInit()
		{
			_func_back_propagate = [](const t_State& result_state, int result_winner, size_t& node_win_time, int node_player)->void
			{
				if (result_winner == node_player)
				{
					node_win_time++;
				}
			};

			_func_ucb_for_parent = [](size_t child_visited_time, size_t child_win_time, size_t parent_visited_time, size_t parent_win_time)->double
			{
				double avg = 1 - ((double)child_win_time / (double)child_visited_time);
				double bound = sqrt(2 * log10(static_cast<double>(parent_visited_time)) / static_cast<double>(child_visited_time));
				return avg + bound;
			};

			_func_default_policy = [](const t_State& original_state, int origianl_player, t_State& result_state, int& result_winner)->void
			{
				t_State temp_state = original_state;
				int temp_player = origianl_player;
				if (debug_model)
				{
					//default policy should not be execute in a end state.
					t_StateInfo original_state_info(temp_state);
					GADT_WARNING_CHECK(original_state_info.is_end_state(), "do default policy in a end state.");
				}

				for (;;)
				{
					t_StateInfo state_info(temp_state);

					//reach a leaf node.
					if (state_info.is_end_state())
					{
						result_state = temp_state;
						result_winner = state_info.winner();
						break;
					}

					//generate next state.
					state_generate_func(temp_state, temp_player, state_info.random_action());
				}
			};

			_func_node_extend = [](const t_State& node_state, size_t node_visited_time, size_t node_win_time)->bool
			{
				return true;
			};

		}

	public:
		inline void SetBackPropagateFunc(FuncBackpropagate func)
		{
			_func_back_propagate = func;
		}
		inline void SetUcbForParentFunc(FuncUcbForParent func)
		{
			_func_ucb_for_parent = func;
		}
		inline void SetDefaultPolicyFunc(FuncDefaultPolicy func)
		{
			_func_default_policy = func;
		}
		inline void SetNodeExtendFunc(FuncNodeExtend func)
		{
			_func_node_extend = func;
		}

		//clear state info in the node after all child nodes of it have been generated.
		inline void SetAutoClearStateInfo(bool b)
		{
			_auto_clear_state_info = b;
		}
	public:

		//Mcts Search.
		MctsSearch() :
			_auto_clear_state_info(true)
		{
			FuncInit();
		}

		//Do MCTS.
		t_Action DoMCTS(t_State original_state, int original_player, size_t iteration_time)
		{
			clock_t f_start = clock();//set timer
			t_MctsNode* root_node = new t_MctsNode(original_state, original_player);
			t_StateInfo root_info = *root_node->state_info();//copy
			if (debug_model)
			{
				GADT_WARNING_CHECK(root_info.is_end_state(), "start from leaf node.");
			}
			//iteration.
			for (size_t i = 0; i < iteration_time; i++)
			{
				t_MctsResult* result = root_node->Extend(
					_func_back_propagate,
					_func_ucb_for_parent,
					_func_default_policy,
					_func_node_extend,
					_auto_clear_state_info
					);
				delete result;
			}
			size_t best_child = root_node->GetBestChild(_func_ucb_for_parent);
			if (debug_model)
			{
				std::cout << "MCTS DEBUG INFO ->" << std::endl << std::endl;
				std::cout << ">> iteration = " << iteration_time << std::endl;
				std::cout << ">> tree size = " << root_node->TreeSizeAsRoot() << std::endl;
				std::cout << ">> time consume = " << (double)(clock() - f_start) / CLOCKS_PER_SEC << " s." << std::endl;
				std::cout << ">> result is action " << best_child << std::endl << std::endl;

				for (size_t i = 0; i < root_node->child_node_ref().size(); i++)
				{
					std::cout << ">> node " << i << ": payoff = " << root_node->child_node_ref()[i]->exception() << " ( ";
					std::cout << root_node->child_node_ref()[i]->win_time() << "/" << root_node->child_node_ref()[i]->visited_time();
					std::cout << " ) for player " << root_node->child_node_ref()[i]->player_index() << std::endl;
				}

				std::cout << std::endl;
				std::cout << ">> node exception = " << root_node->exception() << std::endl << std::endl;
			}

			delete root_node;
			return root_info.action(best_child);
		}
	};

}
