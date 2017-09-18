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

#include "gadtlib.h"
#include "gadt_define.hpp"

#pragma once

namespace gadt
{
	namespace monte_carlo
	{
		/*
		* MonteCarloSetting is the setting of Monte Carlo.
		*
		* MonteCarloSetting() would use default setting.
		* MonteCarloSetting(params) would generate custom setting.
		*
		* more details, see document.
		*/
		struct MonteCarloSetting final: public GameAlgorithmSettingBase
		{
			size_t		thread_num;					//thread num.
			size_t		simulation_times;			//simulation_time;
			size_t		simulation_warning_length;	//if the simulation length out of this value, it would throw a warning if is debug.

			//default setting constructor.
			MonteCarloSetting() :
				GameAlgorithmSettingBase(30,0),
				thread_num(1),
				simulation_warning_length(1000)
			{
			}

			//custom setting constructor.
			MonteCarloSetting(
				double _timeout,
				size_t _thread_num,
				AgentIndex _no_winner_index = 0,
				size_t _simulation_warning_length = 1000
			) :
				GameAlgorithmSettingBase(_timeout, no_winner_index),
				thread_num(_thread_num),
				simulation_warning_length(_simulation_warning_length)
			{
			}

			//output print with str behind each line.
			std::string info() const override
			{
				table::ConsoleTable tb(2, 6);
				tb.set_width({ 12,6 });
				tb.enable_title({ "MONTE CARLO SETTING" });
				tb.set_cell_in_row(0, { { "timeout" },{ console::ToString(timeout) } });
				tb.set_cell_in_row(1, { { "thread_num" },{ console::ToString(thread_num) } });
				tb.set_cell_in_row(4, { { "no_winner_index" },{ console::ToString(no_winner_index) } });
				tb.set_cell_in_row(5, { { "simulation_warning_length" },{ console::ToString(simulation_warning_length) } });
				return tb.output_string();
			}
		};

		/*
		* MonteCarloFuncPackage include necessary function to execute monte-carlo simulation.
		*
		* the only constructor is MonteCarloSetting(params), which include 4 parameters:
		*
		* 1. GetNewStateFunc      = std::function<State(const State&, const Action&)>;
		* 2. MakeActionFunc       = std::function<void(const State&, ActionSet&)>;
		* 3. DetemineWinnerFunc   = std::function<AgentIndex(const State&)>;
		* 4. StateToResultFunc    = std::function<Result(const State&, AgentIndex)>;
		* 5. AllowUpdateValueFunc = std::function<bool(const State&, const Result&)>;
		*
		* more details, see document.
		*/
		template<typename State, typename Action, typename Result, bool _is_debug>
		struct MonteCarloFuncPackage;

		template<typename State, typename Action, typename Result, bool _is_debug>
		class MonteCarloNode
		{
		private:
			using FuncPackage = MonteCarloFuncPackage<State, Action, Result, _is_debug>;

		private:
			const State		_state;				//state of this node.
			AgentIndex		_winner_index;		//the winner index of the state.
			uint32_t		_visited_time;		//how many times that this node had been visited.
			uint32_t		_win_time;			//win time accmulated by the simulation.

		public:
			const State&        state()               const { return _state; }
			AgentIndex          winner_index()        const { return _winner_index; }
			uint32_t            visited_time()        const { return _visited_time; }
			uint32_t            win_time()            const { return _win_time; }

			//increase visited time.
			inline void incr_visited_time()
			{
				_visited_time++;
			}

			//increase win time.
			inline void incr_win_time()
			{
				_win_time++;
			}

			MonteCarloNode(const State& state, const FuncPackage& func_package):
				_state(state),
				_winner_index(func_package.DetemineWinner(_state)),
				_visited_time(0),
				_win_time(0)
			{
			}
		};

		/*
		* MonteCarloFuncPackage include necessary function to execute monte-carlo simulation.
		*
		* the only constructor is MonteCarloSetting(params), which include 5 parameters:
		*
		* 1. GetNewStateFunc      = std::function<State(const State&, const Action&)>;
		* 2. MakeActionFunc       = std::function<void(const State&, ActionSet&)>;
		* 3. DetemineWinnerFunc   = std::function<AgentIndex(const State&)>;
		* 4. StateToResultFunc    = std::function<Result(const State&, AgentIndex)>;
		* 5. AllowUpdateValueFunc = std::function<bool(const State&, const Result&)>;
		*
		* more details, see document.
		*/
		template<typename State, typename Action,typename Result, bool _is_debug>
		struct MonteCarloFuncPackage final : public GameAlgorithmFuncPackageBase<State, Action, _is_debug>
		{
		private:
			using Node = MonteCarloNode<State, _is_debug>;

		public:
			using StateToResultFunc		= std::function<Result(const State&, AgentIndex)>;
			using AllowUpdateValueFunc	= std::function<bool(const State&, const Result&)>;
			using ActionPolicyFunc		= std::function<UcbValue(const &, const Node&)>;
			using DefaultPolicyFunc		= std::function<const Action&(const typename ActionList&)>;
			using ValueForRootNodeFunc	= std::function<UcbValue(const Node&)>;

		public:
			const StateToResultFunc		StateToResult;		//get a result from state and winner.
			const AllowUpdateValueFunc	AllowUpdateValue;	//update values in the node by the result.
			ActionPolicyFunc			ActionPolicy;		//action policy is used for root node to select next action for simulation.
			DefaultPolicyFunc			DefaultPolicy;		//the default policy to select action in simulation.
			ValueForRootNodeFunc		ValueForRootNode;	//select best action of root node after iterations finished.

		public:
			MonteCarloFuncPackage(
				UpdateStateFunc			_UpdateState,
				MakeActionFunc			_MakeAction,
				DetemineWinnerFunc		_DetemineWinner,
				StateToResultFunc		_StateToResult,
				AllowUpdateValueFunc	_AllowUpdateValue
			) :
				GameAlgorithmFuncPackageBase<State, Action, _is_debug>(_UpdateState, _MakeAction, _DetemineWinner),
				StateToResult(_StateToResult),
				AllowUpdateValue(_AllowUpdateValue),
				ActionPolicy([](const Node& parent, const Node& child)->UcbValue {
					UcbValue avg = static_cast<UcbValue>(child.win_time()) / static_cast<UcbValue>(child.visited_time());
					return policy::UCB1(avg, static_cast<UcbValue>(parent.visited_time()), static_cast<UcbValue>(child.visited_time()));
				}),
				DefaultPolicy([](const ActionList& actions)->const Action&{
					GADT_CHECK_WARNING(is_debug(), actions.size() == 0, "MCTS104: empty action set during default policy.");
					return actions[rand() % actions.size()];
				}),
				ValueForRootNode([](const Node& node)->UcbValue {
					return static_cast<UcbValue>(node.visited_time());
				})
			{
			}


		};

		template<typename State, typename Action, typename Result, bool _is_debug>
		class MonteCarloSimulation final :public GameAlgorithmBase<State, Action, Result, _is_debug>
		{
		public:
			using Node = MonteCarloNode<State, Action, Result, _is_debug>;
			using FuncPackage = MonteCarloFuncPackage<State, Action, Result, _is_debug>;
			using ActionList = typename FuncPackage::ActionList;
			using NodeList = std::vector<Node>;
			using CountList = std::vector<size_t>;

		private:
			FuncPackage _func_package;
			MonteCarloSetting _setting;

		private:
			
			//simulation.
			Result Simulation(const State& original_state) const
			{
				State state = original_state;	//copy
				ActionList actions;
				for (size_t i = 0;; i++)
				{
					GADT_CHECK_WARNING(is_debug(), i > _setting.simulation_warning_length, "out of default policy process max length.");

					//detemine winner
					AgentIndex winner = _func_package.DetemineWinner(state);

					//return result if exist.
					if (winner != _setting.no_winner_index)
					{
						return _func_package.StateToResult(state, winner);
					}

					//generate new actions.
					actions.clear();
					_func_package.MakeAction(state, actions);
					GADT_CHECK_WARNING(is_debug(), actions.size() == 0, "empty action list.");

					//choose action by default policy.
					const Action& action = _func_package.DefaultPolicy(actions);

					//state update.
					_func_package.UpdateState(state, action);
				}
				return _func_package.StateToResult(state, _setting.no_winner_index);
			}

			//execute simulation.
			void ExecuteSimulation(const Node& parent, Node& child)
			{
				Result result = Simulation(child.state());
				child.incr_visited_time();
				if (_func_package.AllowUpdateValue(parent, result))
				{
					child.incr_win_time();
				}
			}

			//select best child by action policy.
			void Selection(const Node& parent, NodeList& child_nodes)
			{
				size_t best_index = 0;
				UcbValue best_value = _func_package.ActionPolicy(parent, child_nodes[0]);
				for (size_t i = 1; i < child_nodes.size(); i++)
				{
					UcbValue value = _func_package.ActionPolicy(parent, child_nodes[i]);
					if (value > best_value)
					{
						best_value = value;
						best_index = i;
					}
				}

				//execute simulation in best child
				ExecuteSimulation(parent, child_nodes[best_index]);
			}

			//execute simulatio for all child nodes.
			void ExecuteAllChild(const Node& parent, NodeList& child_nodes)
			{
				for (Node& child : child_nodes)
				{
					ExecuteSimulation(parent, child);
				}
			}

			Action ExecuteMonteCarlo(const State& state, bool enable_action_policy ) const
			{
				//get available actions
				ActionList action_list;
				_func_package.MakeAction(state, action_list);

				//get child nodes
				std::vector<Node> child_nodes;
				for (const Action& act : action_list)
				{
					State temp = state;
					_func_package.UpdateState(temp, act);
					child_nodes.push_back(Node(temp, _func_package));
				}
				size_t sim_time = 1 + _setting.simulation_times / child_states.size();

				//create threads.
				std::vector<std::thread> threads;
				for (size_t thread_id = 0; thread_id < _setting.thread_num; thread_id++)
				{
					if (enable_action_policy)
					{
						threads.push_back(std::thread([&]()->void {

						}));
					}
					else
					{
						//TODO
					}
				}
				//join all threads.
				for (size_t i = 0; i < threads.size(); i++)
				{
					threads[i].join();
				}

				//accumulate all the count list
				CountList total(child_states.size(), 0);
				for (auto count_list : count_lists)
				{
					for (size_t i = 0; i < child_states.size(); i++)
					{
						total[i] += count_list[i];
					}
				}

				//find best action
				size_t best_action_index = 0;
				size_t best_score = total[0];
				for (size_t i = 1; i < total.size(); i++)
				{
					if (total[i] > best_score)
					{
						best_score = total[i];
						best_action_index = i;
					}
				}
				return action_list[best_action_index];
			}

			

		public:

			MonteCarloSimulation(
				typename FuncPackage::UpdateStateFunc		_UpdateState,
				typename FuncPackage::MakeActionFunc		_MakeAction,
				typename FuncPackage::DetemineWinnerFunc	_DetemineWinner,
				typename FuncPackage::StateToResultFunc		_StateToResult,
				typename FuncPackage::AllowUpdateValueFunc	_AllowUpdateValue
			):
				GameAlgorithmBase<State, Action, Result, _is_debug>("Monte Carlo"),
				_func_package(_UpdateState, _MakeAction, _DetemineWinner, _StateToResult, _AllowUpdateValue),
				_setting()
			{
			}

			MonteCarloSimulation(const FuncPackage& func_package) :
				_func_package(func_package),
				_setting()
			{
			}

			Action DoMonteCarlo(const State state, MonteCarloSetting setting)
			{
				_setting = setting;
				return ParallelSimulation(state);
			}
		};
	}
}

