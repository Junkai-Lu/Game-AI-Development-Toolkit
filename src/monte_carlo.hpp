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
		* 1. GetNewStateFunc    = std::function<State(const State&, const Action&)>;
		* 2. MakeActionFunc     = std::function<void(const State&, ActionSet&)>;
		* 3. DetemineWinnerFunc = std::function<AgentIndex(const State&)>;
		* 4. StateToResultFunc  = std::function<Result(const State&, AgentIndex)>;
		*
		* more details, see document.
		*/
		template<typename State, typename Action,typename Result, bool _is_debug>
		struct MonteCarloFuncPackage final : public GameAlgorithmFuncPackageBase<State, Action, _is_debug>
		{
		public:
			using StateToResultFunc		= std::function<Result(const State&, AgentIndex)>;
			using AllowUpdateValueFunc	= std::function<bool(const State&, const Result&)>;
			//using PolicyValueFunc		= std::function<UcbValue(const Node&, const Node&)>;

		public:
			const StateToResultFunc		StateToResult;		//get a result from state and winner.
			const AllowUpdateValueFunc	AllowUpdateValue;	//update values in the node by the result.

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
				AllowUpdateValue(_AllowUpdateValue)
			{
			}
		};

		template<typename State, typename Action, typename Result, bool _is_debug>
		class MonteCarloSimulation final :public GameAlgorithmBase<State, Action, Result, _is_debug>
		{
		public:
			using FuncPackage = MonteCarloFuncPackage<State, Action, Result, _is_debug>;
			using ActionList = typename FuncPackage::ActionList;
			using CountList = std::vector<size_t>;

		private:
			FuncPackage _func_package;
			MonteCarloSetting _setting;

		private:
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

			Action ParallelSimulation(const State& state) const
			{
				//get available actions
				ActionList action_list;
				_func_package.MakeAction(state, action_list);

				//get child state
				std::vector<State> child_states;
				for (const Action& act : action_list)
				{
					child_states.push_back(state);
					_func_package.UpdateState(child_states.back(), act);
				}
				size_t sim_time_each_action = 1 + _setting.simulation_times / child_states.size();

				//create threads.
				std::vector<CountList> count_lists(_setting.thread_num, CountList(child_states.size(), 0));
				std::vector<std::thread> threads;
				for (size_t thread_id = 0; thread_id < _setting.thread_num; thread_id++)
				{
					threads.push_back(std::thread([&](size_t id)->void {
						CountList& count_list = count_lists[id];
						for (size_t i = 0; i < child_states.size(); i++)
						{
							State& child_state = child_states[i];
							for (size_t n = 0; n < sim_time_each_action; n++)
							{
								Result result = Simulation(child_state);
								if (_func_package.AllowUpdateValue(state, result))
								{
									count_list[i] ++;
								}
							}
						}
					}, thread_id));
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

